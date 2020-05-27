#version 460 core

layout(std140, binding=0) buffer primitives{
    vec4 primitiveCoordinates[];
};

struct FlatBvhNode
{
// base aligment             aligned offset
    vec4 min;// 16 byte             0
    vec4 max;// 16 byte            16
    int  order;// 4 byte           32
    int  isLeaf;// 4 byte          36
    int  createdEmpty;// 4 byte    40
    int  leftOrRight;// 4 byte     44
    vec4 indices[12];// 32 byte     48
};

layout(std430, binding=1) buffer TNodes
{
    FlatBvhNode nodes[];
};

struct Material{
    vec4 Ka;
    vec4 Kd;
    vec4 Ks;
    float Ni;
    float shadingModel;
    float shine;
};

layout(std430, binding=2) buffer Materials
{
    Material materials[];
};

struct Light{
    vec3 Le, La;
    vec3 direction;
    vec3 position;
};

struct Ray{
    vec3 orig, dir;
};

struct Hit{
    vec3 orig, normal;
    float u, v;
    float t;
    int mat;
};

uniform Light lights[];
uniform vec3 camera;
uniform sampler2D texture1;

in vec3 pixel;
out vec4 FragColor;

Hit rayTriangleIntersect(Ray ray, vec3 pointA, vec3 pointB, vec3 pointC, int matIndex){

    Hit hit;
    hit.t=-1;
    float t; float u; float v;
    vec3 pApB = pointB - pointA;
    vec3 pApC = pointC - pointA;
    vec3 vec90 = cross(ray.dir, pApC);
    float determinant = dot(vec90, pApB);

    /* if (abs(det) < 0.002){
         hit.t=-1;
         return hit;
     }*/

    float determinantInv = 1 / determinant;

    vec3 vecT = ray.orig - pointA;
    u = determinantInv* dot(vecT, vec90);
    if (u < 0 || u > 1){
        hit.t=-1;
        return hit;
    }

    vec3 vecQ = cross(vecT, pApB);
    v = determinantInv*dot(vecQ, ray.dir);
    if (v < 0 || u + v > 1) {
        hit.t=-1;
        return hit;
    }

    hit.t = dot(pApC, vecQ) * determinantInv;
    hit.orig=ray.orig+normalize(ray.dir)*hit.t;
    hit.normal= normalize(cross(pApB, pApC));

    hit.u=u;
    hit.v=v;
    hit.mat=matIndex;

    return hit;
}

vec4 getCoordinatefromIndices(float index){
    return primitiveCoordinates[int(index)];
}

bool rayIntersectWithBox(vec4 boxMin, vec4 boxMax, Ray r) {
    vec3 invdir = 1.0 / r.dir.xyz;
    vec3 f = (boxMax.xyz - r.orig.xyz) * invdir;
    vec3 n = (boxMin.xyz - r.orig.xyz) * invdir;

    vec3 tmax = f * sign(invdir);
    vec3 tmin = n * sign(invdir);

    return tmin.x < tmax.x && tmin.y < tmax.y && tmin.z < tmax.z;
}

Hit traverseBvhNode(Ray ray, FlatBvhNode node){
    Hit besthit;
    besthit.t=-1;
    bool hit;
    Hit hitreal;
    int i=0;

    while (i<=nodes.length()) {
        // Ha találat van és leaf a node, akkor bejárjuk, hogy van-e intersection.
        if (nodes[i].isLeaf==1){
            for (int j=0;j<nodes[i].indices.length();j++){
                if (mod(nodes[i].indices[j].x, 1)==0 && mod(nodes[i].indices[j].y, 1)==0 && mod(nodes[i].indices[j].z, 1)==0){
                    vec3 TrianglePointA=getCoordinatefromIndices(nodes[i].indices[j].x).xyz;
                    vec3 TrianglePointB=getCoordinatefromIndices(nodes[i].indices[j].y).xyz;
                    vec3 TrianglePointC=getCoordinatefromIndices(nodes[i].indices[j].z).xyz;

                    hitreal=rayTriangleIntersect(ray, TrianglePointA, TrianglePointB, TrianglePointC, int(nodes[i].indices[j].w));

                    if (hitreal.t==-1){ continue; }

                    if (hitreal.t>0 && (besthit.t>hitreal.t || besthit.t<0)){
                        besthit=hitreal;
                    }
                }
            }
            // Ha bal oldali a leaf, akkor átmegyünk a jobb oldaliba.
            if (nodes[i].leftOrRight==0){
                i=i+1;
                continue;
            }

            // Ha jobb oldali a leaf, akkor feljebb megyünk addig, amíg a szülőnek nem lesz jobb oldali testvére
            else if (nodes[i].leftOrRight==1){

                int id=int(ceil(i-2)/2);
                FlatBvhNode parent=nodes[id];

                while (parent.leftOrRight==1){
                    parent=nodes[int(ceil(parent.order-2)/2)];
                    if (parent.order==0){
                        return besthit;
                    }
                }
                i = parent.order+1;
                continue;
            }
        }

        hit = rayIntersectWithBox(nodes[i].min, nodes[i].max, ray);

        if (hit) {
            // Ha találat van és nem leaf a node, akkor megyünk lejjebb.
            if (nodes[i].isLeaf==0){
                i=2*i+1;
                continue;
            }
        }

        else {
            // Ha nincs találat és a root node-nál vagyunk, akkor kilépés.
            if (nodes[i].order==0){
                break;
            }

            // Ha nincs találat és nem a root node-nál vagyunk és bal oldali node-nál vagyunk.
            if (nodes[i].leftOrRight==0) {
                i=i+1;
                continue;
            }

            // Ha nincs találat és nem a root node-nál vagyunk és jobb oldali node-nál vagyunk.
            else if (nodes[i].leftOrRight==1){
                FlatBvhNode parent=nodes[int(ceil(i-2)/2)];

                while (parent.leftOrRight==1){
                    parent=nodes[int(ceil(parent.order-2)/2)];
                    if (parent.order==0){
                        if (parent.order==0){
                            return besthit;
                        }
                    }
                }
                i = parent.order+1;
                continue;
            }
        }
    }
    return besthit;
}


Hit traverseBvhTree(Ray ray){
    return traverseBvhNode(ray, nodes[0]);
}




vec3 Fresnel(vec3 F0, float cosTheta) {
    return F0 + (vec3(1, 1, 1) - F0) * pow(1-cosTheta, 5);
}

float schlickApprox(float Ni, float cosTheta){
    float F0=pow((1-Ni)/(1+Ni), 2);
    return F0 + (1 - F0) * pow((1 - cosTheta), 5);
}


vec3 trace(Ray ray){
    vec3 weight = vec3(1, 1, 1);
    const float epsilon = 0.0001f;
    vec3 color = vec3(0, 0, 0);

    int tracingDepth=5;

    for (int i=0; i < tracingDepth; i++){
        Hit hit=traverseBvhTree(ray);
        if (hit.t<0){ return weight * lights[0].La; }

        vec4 textColor = texture(texture1, vec2(hit.u, hit.v));
        Ray shadowRay;
        shadowRay.orig = hit.orig + hit.normal * epsilon;
        shadowRay.dir  = normalize(lights[0].direction);


        // Ambient Light
        color+= materials[hit.mat].Ka.xyz * lights[0].La*textColor.xyz * weight;

        // Diffuse light
        float cosTheta = dot(hit.normal, normalize(lights[0].direction));// Lambert-féle cosinus törvény alapján.
        if (cosTheta>0 && traverseBvhTree(shadowRay).t<0) {

            color +=lights[0].Le * materials[hit.mat].Kd.xyz * cosTheta * weight;

            vec3 halfVec = normalize(-ray.dir + lights[0].direction);
            float cosDelta = dot(hit.normal, halfVec);

            // Specular light
            if (cosDelta > 0){
                color +=weight * lights[0].Le * materials[hit.mat].Ks.xyz * pow(cosDelta, materials[hit.mat].shine); }
        }

        if (materials[hit.mat].shadingModel == 1) {
            weight *= schlickApprox(materials[hit.mat].Ni, cosTheta);
            ray.orig=hit.orig+hit.normal*epsilon;
            ray.dir=reflect(ray.dir, hit.normal);
        } else return color;
    }
    return color;
}


void main()
{
    Ray ray;
    ray.orig = camera;
    ray.dir = normalize(pixel - camera);
    FragColor = vec4(trace(ray), 1);

    // FragColor = vec4(nodes[4].isLeaf, 1, 1, 1);
}