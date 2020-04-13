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


struct Light{
    vec3 Le, La;
    vec3 direction;
    vec3 position;

};

uniform Light lights;

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
    hit.normal= cross(v0v1,v0v2);
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

        vec3 TrianglePointA=vec3(0.2,0.6,0.5);
        vec3 TrianglePointB=vec3(0.5,0.7,0.4);
        vec3 TrianglePointC=vec3(0.6,0.8,0.3);
        Hit hit=rayTriangleIntersect(ray,TrianglePointA,TrianglePointB,TrianglePointC);

       // if(hit.t>0 && (besthit.t>hit.t|| besthit.t<0)){
         //   besthit=hit;
        //}



    return besthit;
}

vec3 trace(Ray ray){
    vec3 color;
  /*  vec3 La=vec3(0.3,0.3,0.3);
    vec3 Le=vec3(4.0,4.0,4.0);
*/
    vec3 ka=vec3(0.5215,0.1745,0.0215);


    Hit hit;
  //  hit=firstIntersect(ray);
  /*  if (hit.t==-1){
        return lights[0].La;
    }*/
    color=lights.La;

    /*
    for (int i=0;i<lights.length();i++){
        Ray shadowRay;
        shadowRay.orig=hit.orig;
        shadowRay.dir=lights[0].direction;

        Hit shadowHit=firstIntersect(shadowRay);

        if (shadowHit.t<0 || shadowHit.t > length(shadowRay.orig-lights[0].position)){
            color+=lights[i].Le;
        }
    }*/
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