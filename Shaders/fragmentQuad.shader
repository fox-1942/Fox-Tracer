#version 460 core

layout(std140, binding=2) buffer primitives{
    vec3 primitiveCoordinates[];
};

layout(std140, binding=3) buffer indices{
    vec3 indicesC[];
};

in       vec2       TexCoords;
out      vec4       FragColor;
in  vec3 p;
uniform vec3 wEye;
uniform  sampler2D  texture_diffuse1;

struct Ray{
    vec3 orig, dir;
};

struct Hit{
    vec3 orig, dir;
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
        return hit; // Culling is off
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
    return hit;
}


vec3 getCoordinatefromIndices(float index){
    vec3 back=vec3(-1, -1, -1);
    for (int i=0; i < primitiveCoordinates.length();i++){
        if (i==index){
            back=primitiveCoordinates[i];
            break;
        }
    }
    return back;
}


Hit firstIntersect(Ray ray){
    Hit besthit;
    besthit.t=-1;
    for (int i=0;i<indicesC.length();i++){
        vec3 TrianglePointA=getCoordinatefromIndices(indicesC[i].x);
        vec3 TrianglePointB=getCoordinatefromIndices(indicesC[i].y);
        vec3 TrianglePointC=getCoordinatefromIndices(indicesC[i].z);
        Hit hit=rayTriangleIntersect(ray,TrianglePointA,TrianglePointB,TrianglePointC);

        if(hit.t>0 && (besthit.t>hit.t|| besthit.t<0)){
            besthit=hit;
        }

    }

    return besthit;
}

vec3 trace(Ray ray){
    vec3 color=vec3(0, 0, 0);
    Hit hit;
    hit=firstIntersect(ray, TrianglePointA, TrianglePointB, TrianglePointC);
    if (hit.t==-1){
        return La;
    }
    color=ka*La;

    for (int i=0;i<lights.length();i++){
        Ray shadowRay;
        shadowRay.orig=hit.orig;
        shadowRay.dir=lights[i]-hit.orig;

        Hit shadowHit=firstIntersect(shadowRay);

        if (shadowHit.t<0 || shadowHit.t > abs(shadowRay.orig-hit.orig)){
            color+=lights[i].Le;
        }
    }
    return color;
}

void main()
{
    Ray ray;
    ray.orig = wEye;
    ray.dir = normalize(p - wEye);
    
    FragColor = vec4(TrianglePointC.z-191.5, 0, 0, 1);

    // FragColor=vec4(trace(), 1.0f);
    // FragColor = vec4(indicesC[21].x, indicesC[21].y-41.5, indicesC[21].z, 1.0f);
}