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
    vec3 indices[2];// 32 byte   48
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


Hit firstIntersect(Ray ray, FlatBvhNode node){
    Hit besthit;
    besthit.t=-1;
    for (int j=0;j<node.indices.length();j++){
        vec3 TrianglePointA=getCoordinatefromIndices(node.indices[j].x);
        vec3 TrianglePointB=getCoordinatefromIndices(node.indices[j].y);
        vec3 TrianglePointC=getCoordinatefromIndices(node.indices[j].z);
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
    vec3 tbot = r.dir * (boxMin - r.orig);
    vec3 ttop = r.dir * (boxMax - r.orig);
    vec3 tmin = min(ttop, tbot);
    vec3 tmax = max(ttop, tbot);
    vec2 t = max(tmin.xx, tmin.yz);
    float t0 = max(t.x, t.y);
    t = min(tmax.xx, tmax.yz);
    float t1 = min(t.x, t.y);

    /* Hit hit;
     hit.tmin = t0;
     hit.tmax = t1;*/

    return t1 > max(t0, 0.0);
}

// Traverse one Node
/*
Hit traverseBvhNode(Ray ray, FlatBvhNode node){
    int node_index_next = 0;

    for (int node_index = 0; node_index < nodes.size(); node_index++) {
        if (node_index != node_index_next) {
            continue;
        }

        bool hit = rayIntersectWithBox(nodes[node_index].min,nodes[node_index].max, ray);
        bool leaf = nodes[node_index].indices.size() > 0;

        if (hit) {
            if(nodes[node_index].isLeaf && nodes[node_index].right){
                nodes[]

            }


            node_index_next = nodes[2*nodes[node_index]+1].order; // hit link
        } else {
            node_index_next = nodes[node_index].links.y; // miss link
        }

        if (hit && leaf) {
            uint a = boxes[box_index].ids_offset;
            uint b = a + boxes[box_index].ids_count;

            for (uint j = a; j < b; j++) {
                uint triangle_id = triangle_references[j];
                // triangle intersection code ...
            }
        }
    }
}*/

Hit traverseBvhTree(Ray ray){
    if (rayIntersectWithBox(nodes[0].min, nodes[0].max, ray)){
        return traverseBvhNode(ray, nodes[0]);
    }
    Hit no;
    no.t=-1;
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

    //FragColor = vec4(nodes[1].min.x, 1, 1, 1);
}