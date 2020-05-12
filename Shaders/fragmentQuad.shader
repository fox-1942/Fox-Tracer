#version 460 core

layout(std140, binding=0) buffer primitives{
    vec3 primitiveCoordinates[];
};

layout(std140, binding=1) buffer indices{
    vec3 indicesC[];
};

struct FlatBvhNode
{
// base aligment                aligned offset
    vec3 min;// 16 byte          0
    vec3 max;// 16 byte          16
    int order;// 4 byte          32
    int isLeaf;// 4 byte         36
    int createdEmpty;// 4 byte   40
    int leftOrRight;
    vec3 indices[400];// 32 byte   48
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

struct IntersectionPoint{
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

vec3 getCoordinatefromIndices(float index){
    return primitiveCoordinates[int(index)];
}


Hit firstIntersect(Ray ray, int i){
    Hit besthit;
    besthit.t=-1;
    for (int j=0;j<nodes[i].indices.length();j++){
        vec3 TrianglePointA=getCoordinatefromIndices(nodes[i].indices[j].x);
        vec3 TrianglePointB=getCoordinatefromIndices(nodes[i].indices[j].y);
        vec3 TrianglePointC=getCoordinatefromIndices(nodes[i].indices[j].z);
        Hit hit=rayTriangleIntersect(ray, TrianglePointA, TrianglePointB, TrianglePointC);

        if (hit.t==-1){ continue; }

        if (hit.t>0 && (besthit.t>hit.t || besthit.t<0)){
            besthit=hit;
        }
    }
    return besthit;
}

FlatBvhNode leftChild(FlatBvhNode node){
    return nodes[2*node.order+1];
}

FlatBvhNode rightChild(FlatBvhNode node){
    return nodes[2*node.order+2];
}

bool rayIntersectWithBox(const vec3 boxMin, const vec3 boxMax, const Ray r) {
    vec3 invdir = 1.0 / r.dir.xyz;
    const vec3 f = (boxMax.xyz - r.orig.xyz) * invdir;
    const vec3 n = (boxMin.xyz - r.orig.xyz) * invdir;

    const vec3 tmax = max(f, n);
    const vec3 tmin = min(f, n);

    const float t1 = min(tmax.x, min(tmax.y, tmax.z));
    const float t0 = max(max(tmin.x, max(tmin.y, tmin.z)), 0.0f);

    return t1 >= t0;
}

// Traverse one Node

Hit traverseBvhNode(Ray ray, FlatBvhNode node){
    Hit result;


    int next = 0;
    for (int i = 0; i < nodes.length(); i++) {


        if (i != next) {
            continue;
        }


        bool hit = rayIntersectWithBox(nodes[i].min, nodes[i].max, ray);

        if (nodes[i].createdEmpty==1){
            hit=false;
        }

        if (hit) {
            if (nodes[i].isLeaf==1 && nodes[i].createdEmpty!=1){ return firstIntersect(ray, i); }

            next = 2*i+1;
        }

        else if (!hit) {

            if (nodes[i].leftOrRight==0){

                next = i+1; }
            else if (nodes[i].leftOrRight==1){ next = int(ceil(i-2)/2); }
        }
    }
    result.t=-4;
    return result;
}


Hit traverseBvhTree(Ray ray){
    Hit no;
    no.t=-4;

    if (rayIntersectWithBox(nodes[0].min, nodes[0].max, ray)){
        return traverseBvhNode(ray, nodes[0]);
    }
    return no;
}

bool shadowIntersect(Ray ray){
    for (int i=0;i<indicesC.length();i++){
        vec3 TrianglePointA=getCoordinatefromIndices(indicesC[i].x);
        vec3 TrianglePointB=getCoordinatefromIndices(indicesC[i].y);
        vec3 TrianglePointC=getCoordinatefromIndices(indicesC[i].z);
        if (rayTriangleIntersect(ray, TrianglePointA, TrianglePointB, TrianglePointC).t>0){
            return true;
        }
    }
    return false;
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
    if (cosTheta > 0 && !shadowIntersect(shadowRay)){
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

    //FragColor = vec4(nodes[1].leftOrRight, 1, 1, 1);
}