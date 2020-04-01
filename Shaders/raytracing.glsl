#version 460 core

layout(binding = 0, rgba32f) uniform image2D framebufferImage;

layout (local_size_x = 16, local_size_y = 16) in;

struct Sphere {
    vec3 center;
    float radius;
};

struct Hit {
    float t;
    vec3 position;
    vec3 normal;
    float material;
};

struct Ray{
    vec3 start;
    vec3 dir;

};



Hit intersect(const Sphere object, const Ray ray) {
    Hit hit;
    hit.t = -1;// we first suppose there is no intersection
    vec3 dist = ray.start - object.center;// distance from center to the start of ray

    float a = dot(ray.dir, ray.dir);
    float b = dot(dist, ray.dir) * 2.0f;
    float c = dot(dist, dist) - object.radius * object.radius;

    float discr = b * b - 4.0f * a * c;// discriminant

    if (discr < 0)// There is no intersection
    return hit;

    float sqrt_discr = sqrt(discr);
    float t1 = (-b + sqrt_discr) / 2.0f / a;
    float t2 = (-b - sqrt_discr) / 2.0f / a;

    if (t1 <= 0)
    return hit;// There is no intersection or just touching the surface

    hit.t = (t2 > 0) ? t2 : t1;
    hit.position = ray.start + ray.dir * hit.t;
    hit.normal = (hit.position - object.center) / object.radius;

    return hit;
}

Hit firstIntersect(Ray ray) {
    Hit bestHit;
    bestHit.t = -1;

    Sphere selected;

    for (int i = 0; i < 2; i++) {

        Hit hit = intersect(objects[i], ray);//  hit.t < 0 if no intersection

        if (hit.t > 0 && (bestHit.t < 0 || hit.t < bestHit.t))
        bestHit = hit;
        selected=objects[i];

    }
    N=bestHit.position-selected.center;

    return bestHit;
}


void trace(){



    firstintersect();

}

void main(){
    ivec2 px = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(framebufferImage);

    if (any(greaterThanEqual(px, size)))
    return; // <- no work to do, return.

    vec2 p = (vec2(px) + vec2(0.5)) / vec2(size);

    Shere[2] spheres;

    sphere[0].radius=0.5;
    sphere[0].center=vec3(1.0, 1.5, 1.0);

    sphere[1].radius=0.5;
    sphere[1].center=vec3(2.0, 1.5, 1.0);

    trace();
}