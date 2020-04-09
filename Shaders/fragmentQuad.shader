#version 460 core

layout(std140, binding=2) uniform primitives{
    vec3 primitiveCoordinates[3430];

};

in       vec2       TexCoords;
out      vec4       FragColor;
uniform  sampler2D  texture_diffuse1;

struct Ray{
    vec3 orig, dir;
};

struct Hit{
    vec3 orig, dir;
};

vec3 outIntersectionPoint;

bool rayTriangleIntersect(Ray ray, vec3 v0,  vec3 v1,  vec3 v2, float t, float u, float v){

    vec3 v0v1 = v1 - v0;
    vec3 v0v2 = v2 - v0;
    vec3 pvec = cross(ray.dir,v0v2);
    float det = dot(v0v1,pvec);

    if (abs(det) < 0.008) return false; // Culling is off

    float invDet = 1 / det;

    vec3 tvec = ray.orig - v0;
    u = dot(tvec,pvec) * invDet;
    if (u < 0 || u > 1) return false;

    vec3 qvec = cross(tvec,v0v1);
    v = dot(ray.dir,qvec) * invDet;
    if (v < 0 || u + v > 1) return false;

    t = dot(v0v2,qvec) * invDet;

    return true;
}


void firstIntersect(Ray ray){
    for (int i=0;i<primitiveCoordinates.length();i=i+3){
        float t, u, v;
        if (rayTriangleIntersect(ray, primitiveCoordinates[i], primitiveCoordinates[i+1],primitiveCoordinates[i+2], t, u, v)){

        }

    }

}

void trace(){


}



void main()
{

   // FragColor=vec4(trace(), 1.0f);
    FragColor = vec4(primitiveCoordinates[2].x-575.5f, primitiveCoordinates[2].y-56.0f, primitiveCoordinates[2].z-704.0f, 1.0f);
}