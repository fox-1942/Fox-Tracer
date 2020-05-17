#version 460 core

layout(std140, binding=0) buffer primitives{
    vec4 primitiveCoordinates[];
};

layout(std140, binding=1) buffer indices{
    vec4 indicesC[];
};

struct FlatBvhNode
{
// base aligment                aligned offset
    vec4 min;// 16 byte              0
    vec4 max;// 16 byte             16
    int  order;// 4 byte            32
    int  isLeaf;// 4 byte           36
    int  createdEmpty;// 4 byte     40
    int  leftOrRight;
    vec4 indices[10];// 32 byte     48
};

layout(std430, binding=2) buffer TNodes
{
    FlatBvhNode nodes[];
};

out vec4 FragColor;
in vec3 p;
uniform vec3 wEye;

struct Light{
    vec3 Le, La;
    vec3 direction;
    vec3 position;
};

uniform Light lights[];

struct Ray{
    vec3 orig, dir;
};

struct Hit{
    vec3 orig, dir, normal;
    float t;
};


Hit rayTriangleIntersect(Ray ray, vec3 v0, vec3 v1, vec3 v2){

    Hit hit;
    float t; float u; float v;
    vec3 v0v1 = v1 - v0;
    vec3 v0v2 = v2 - v0;
    vec3 pvec = cross(ray.dir, v0v2);
    float det = dot(v0v1, pvec);

    if (abs(det) < 0.008){
        hit.t=-1;
        return hit;// Culling is off
    }
    float invDet = 1 / det;

    vec3 tvec = ray.orig - v0;
    u = dot(tvec, pvec) * invDet;
    if (u < 0 || u > 1){
        hit.t=-1;
        return hit;
    }

    vec3 qvec = cross(tvec, v0v1);
    v = dot(ray.dir, qvec) * invDet;
    if (v < 0 || u + v > 1) {
        hit.t=-1;
        return hit;
    }

    hit.t = dot(v0v2, qvec) * invDet;
    hit.normal= cross(v0v1, v0v2);
    return hit;
}

vec4 getCoordinatefromIndices(float index){
    return primitiveCoordinates[int(index)];
}


FlatBvhNode leftChild(FlatBvhNode node){
    return nodes[2*node.order+1];
}

FlatBvhNode rightChild(FlatBvhNode node){
    return nodes[2*node.order+2];
}

bool rayIntersectWithBox(const vec4 boxMin, const vec4 boxMax, const Ray r) {
    vec3 invdir = 1.0 / r.dir.xyz;
    const vec3 f = (boxMax.xyz - r.orig.xyz) * invdir;
    const vec3 n = (boxMin.xyz - r.orig.xyz) * invdir;

    const vec3 tmax = f * sign(invdir);
    const vec3 tmin = n * sign(invdir);

    return tmin.x < tmax.x && tmin.y < tmax.y && tmin.z < tmax.z;
}

// Traverse one Node
/*
Hit traverseBvhNode(Ray ray, FlatBvhNode node){

    Hit besthit;
    besthit.t=-1;

    int next = 0;
    for (int i = 0; i < nodes.length(); i++) {

        if (nodes[i].isLeaf==1 && nodes[i].createdEmpty==0){

            for (int j=0;j<nodes[i].indices.length();j++){

                if (mod(nodes[i].indices[j].x, 1)==0 && mod(nodes[i].indices[j].y, 1)==0 && mod(nodes[i].indices[j].z, 1)==0){

                    vec3 TrianglePointA=getCoordinatefromIndices(nodes[i].indices[j].x).xyz;
                    vec3 TrianglePointB=getCoordinatefromIndices(nodes[i].indices[j].y).xyz;
                    vec3 TrianglePointC=getCoordinatefromIndices(nodes[i].indices[j].z).xyz;

                    Hit hit=rayTriangleIntersect(ray, TrianglePointA, TrianglePointB, TrianglePointC);

                    if (hit.t==-1){ continue; }

                    if (hit.t>0 && (besthit.t>hit.t || besthit.t<0)){
                        besthit=hit;
                    }
                }
                else { continue; }
            }
        }
    }
    return besthit;
}*/

Hit firstIntersect(Ray ray, int i){
    Hit besthit;
    besthit.t=-1;
    for (int j=0;j<nodes[i].indices.length();j++){
        vec3 TrianglePointA=getCoordinatefromIndices(nodes[i].indices[j].x).xyz;
        vec3 TrianglePointB=getCoordinatefromIndices(nodes[i].indices[j].y).xyz;
        vec3 TrianglePointC=getCoordinatefromIndices(nodes[i].indices[j].z).xyz;
        Hit hit=rayTriangleIntersect(ray, TrianglePointA, TrianglePointB, TrianglePointC);

        if (hit.t==-1){ continue; }

        if (hit.t>0 && (besthit.t>hit.t || besthit.t<0)){
            besthit=hit;
        }
    }
    return besthit;
}

Hit traverseBvhNode(Ray ray, FlatBvhNode node){
    Hit besthit;
    besthit.t=-1;

    int db=0;
    Hit hitreal;

    int i=0;
    while (i<=nodes.length()) {


        bool hit= rayIntersectWithBox(nodes[i].min, nodes[i].max, ray);
        if (i==3 || i==6 || i==9|| i==10 || i==11 || i==12|| i==6){
            hit=true;
        }


        if (hit) {


            // Ha találat van és nem leaf a node, akkor megyünk lejjebb.
            if (nodes[i].isLeaf==0){
                i=2*i+1;
                continue;
            }

            // Ha találat van és leaf a node, akkor bejárjuk, hogy van-e intersection.
            else if (nodes[i].isLeaf==1){
                db++;

                for (int j=0;j<nodes[i].indices.length();j++){
                    if (mod(nodes[i].indices[j].x, 1)==0 && mod(nodes[i].indices[j].y, 1)==0 && mod(nodes[i].indices[j].z, 1)==0){

                        vec3 TrianglePointA=getCoordinatefromIndices(nodes[i].indices[j].x).xyz;
                        vec3 TrianglePointB=getCoordinatefromIndices(nodes[i].indices[j].y).xyz;
                        vec3 TrianglePointC=getCoordinatefromIndices(nodes[i].indices[j].z).xyz;

                        hitreal=rayTriangleIntersect(ray, TrianglePointA, TrianglePointB, TrianglePointC);

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
                        if (parent.order==0 || parent.order==-0){


                            break; }
                    }
                    i = parent.order+1;
                    continue;
                }
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
                int id=int(ceil(i-2)/2);
                FlatBvhNode parent=nodes[id];

                while (parent.leftOrRight==1){
                    parent=nodes[int(ceil(parent.order-2)/2)];
                    if (parent.order==0 || parent.order==-0){ break; }
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


vec3 trace(Ray ray){
    vec3 color= vec3(0, 0, 0);
    vec3 ka= vec3(0.135, 0.2225, 0.1575);
    vec3 kd= vec3(0.54, 0.89, 0.63);

    

    Hit hit=traverseBvhTree(ray);

    if (hit.t==-1){ return lights[0].La; }

    color=lights[0].La*ka;

    Ray shadowRay;
    shadowRay.orig=hit.orig+hit.normal*0.001f;
    shadowRay.dir=lights[0].direction;
    float cosTheta = dot(hit.normal, lights[0].direction)/(length(hit.normal)*length(lights[0].direction));
    if (cosTheta > 0){
        color+=lights[0].Le*cosTheta*kd;
        float cosDelta=dot(hit.normal, normalize(-ray.dir + lights[0].direction));
        if (cosDelta>0){
            color=color+lights[0].Le*vec3(0.316228, 0.316228, 0.316228)*pow(0.1, cosDelta);
        }
    }
    return color;
}

void main()
{
    Ray ray;
    ray.orig = wEye;
    ray.dir = normalize(p - wEye);
    FragColor = vec4(trace(ray), 1);

    // FragColor = vec4(nodes[4].isLeaf, 1, 1, 1);
}