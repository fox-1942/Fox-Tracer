#version 460 core
/*
#define MIN(A, B); ((A) < (B) ? (A) : (B));
#define MAX(A, B); ((A) > (B) ? (A) : (B));*/

layout(std140, binding=2) buffer primitives{
    vec3 primitiveCoordinates[];
};

layout(std140, binding=3) buffer indices{
    vec3 indicesC[];
};

in       vec2       TexCoords;
out      vec4       FragColor;
in       vec3 p;
uniform vec3 wEye;
uniform  sampler2D  texture_diffuse1;

struct BvhNode{
    float min, max;
    int childrenIndices[2];

// If node is a leaf
    bool isLeaf;
    vec3 indices;
} ;

BvhNode nodes[100];

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

vec3 outIntersectionPoint;

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


Hit firstIntersect(Ray ray, BvhNode node){
    Hit besthit;
    besthit.t=-1;
    for (int j=0;j<length(node.indices);j++){
      /*  vec3 TrianglePointA=getCoordinatefromIndices(node.indices[j].x);
        vec3 TrianglePointB=getCoordinatefromIndices(node.indices[j].y);
        vec3 TrianglePointC=getCoordinatefromIndices(node.indices[j].z);
        Hit hit=rayTriangleIntersect(ray, TrianglePointA, TrianglePointB, TrianglePointC);
*/
        if (hit.t==-1){ continue; }

        if (hit.t>0 && (besthit.t>hit.t|| besthit.t<0)){
            besthit=hit;
        }
    }
    return besthit;
}

// Traverse one Node
Hit traverseBvhNode(Ray ray, BvhNode node){
    Hit resultHit; resultHit.t=-2;

    // Ha már metszette a node-ot.
    if (rayIntersectWithBox(ray, node)){
        if (!node.isLeaf){
            Hit it1 = rayIntersectWithBox(node.childrenIndices[0]);
            Hit it2 = rayIntersectWithBox(node.childrenIndices[1]);

            //Mindkét gyereket metszi.
            if (it1.t>0 && it2.t>0) {
                // Végijárom a bal gyereket
                Hit hitFromLeftChild=traverseBvhNode(ray, node.childrenIndices[0]);

                if (hitFromLeftChild.t>0 && hitFromLeftChild.t<it2.t){ return hitFromLeftChild; }

                // Végigjárom a jobb gyereket
                Hit hitFromRightChild=traverseBvhNode(ray, node.childrenIndices[1]);

                // A közelebbi metszéssel térek vissza
                if (hitFromLeftChild.t>0 && (hitFromLeftChild.t<hitFromRightChild.t || hitFromRightChild.t==-1)){ return hitFromLeftChild; }

                else if (hitFromRightChild.t>0 && (hitFromRightChild.t<hitFromLeftChild.t ||  hitFromLeftChild.t==-1)){ return hitFromRightChild; }
            }

            //Ha a Left gyereket metszi
            else if (it1.t>0){ resultHit=traverseBvhNode(); }

            // Ha a Right gyereket metszi.
            else if (it2.t>0){ resultHit=traverseBvhNode(); }

            // Ha egyik gyereket sem metszi, csak áthalad mindkettőn.
            else if (it1.t==-1 && it2.t==-1){ return resultHit; }
        }
        else { resultHit=firstIntersect(ray, node); }
    }
    return resultHit;
}

void traverseBvhTree(Ray ray){
    for (int i;i<nodes.size();i++){
        traverseBvhNode(ray, nodes[i]);
    }
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

    Hit hit;
    hit=firstIntersect(ray);
    if (hit.t==-1){
        return lights[0].La;
    }
    color=lights[0].La*ka;

    Ray shadowRay;
    shadowRay.orig=hit.orig+hit.normal*0.001f;
    shadowRay.dir=lights[0].direction;
    float cosTheta = dot(hit.normal, lights[0].direction)/(length(hit.normal)*length(lights[0].direction));
    if (cosTheta > 0 && shadowIntersect(shadowRay)){
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

    // FragColor=vec4(trace(), 1.0f);
    // FragColor = vec4(indicesC[21].x, indicesC[21].y-41.5, indicesC[21].z, 1.0f);
}