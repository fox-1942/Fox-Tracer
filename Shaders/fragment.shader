#version 460 core

struct Ray {
    vec3 start;
    vec3 dir;
};


struct Material {
    vec3 ka, kd, ks;
    float  shininess;
};


struct Hit {
    float t;
    vec3 position;
    vec3 normal;
};

struct Sphere {
    vec3 center;
    float radius;
};


uniform vec3 cameraPosition;
uniform int numberOfObjects;
uniform Sphere objects[300];

in  vec3 p;
out vec4 fragColor;


float angleDegCalculator(vec3 first, vec3 second){
    vec3 dotProduct = dot(first, second);

    vec3 firstLength = length(first);
    vec3 secondLength = length(second);

    float angle = acos(dotProduct / firstLength * secondLength);
    return angle;
}

float fresnelCalculator(float fractionIndexOfMedium1, float fractionIndexOfMedium2, vec3 I, vec3 N){

    Theta1=angleDegCalculator(I, N);
    cosTheta1=cos(Theta1);

    if (sin(Theta1)>=1){
        // Total reflection happens
        float fresnelReflected=1;
        return fresnelReflected;
    }

    else{
    // Cases when there is refraction and reflaction as well, not only reflaction.

    // Based on Snell's law:
    Theta2=sin(Theta1) * fractionIndexOfMedium1/fractionIndexOfMedium2;

    cosTheta2=cos(Theta2);

    float fresnelReflectedLongitudinal=pow(((fractionIndexOfMedium2 * cosTheta1) - (fractionIndexOfMedium1 * cosTheta2)) / ((fractionIndexOfMedium2 * cosTheta1) + (fractionIndexOfMedium1 * cosTheta2)),2);

    float fresnelReflectedTransversal=pow(((fractionIndexOfMedium1 * cosTheta1) - (fractionIndexOfMedium2 *cosTheta2)) / ((fractionIndexOfMedium1 * cosTheta1) + (fractionIndexOfMedium2 * cosTheta2)),2);

    float fresnelReflected=0.5*(fresnelReflectedLongitudinal+fresnelReflectedTransversal);

    float fresnelRefractedComponent= 1 - fresnelReflected;

    return fresnelReflected;

    }
}

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

    for (int i = 0; i < numberOfObjects; i++) {

        Hit hit = intersect(objects[i], ray);//  hit.t < 0 if no intersection

        if (hit.t > 0 && (bestHit.t < 0 || hit.t < bestHit.t))
        bestHit = hit;
    }

    return bestHit;
}


Vec3f reflectedRayDir(const Vec3 I, const Vec3f N){
    Vec3f result= I - 2 * dotProduct(I, N) * N;
    return result;
}


Vec3f refractedRayDir(const Vec3 I, const Vec3f N, const float secondMediumRefractionIndex)
{
    float cosTheta1 = clamp(-1, 1, dotProduct(I, N));
    float firstMediumRefractionIndex = 1;
    float sMRI = secondMediumRefractionIndex;

    Vec3f n = N;

    if (cosTheta1 < 0)
    {
        cosTheta1 = -cosTheta1;// the ray coming from outside
    }
    else
    {
        std::swap(firstMediumRefractionIndex, sMRI);
// the ray coming from inside and we have to change the fraction indices

n= -N;// we have to multiply the normal by -1 because we are upside-down
}
}

float n_refraction_fraction = firstMediumRefractionIndex / sMRI;

// examining if there is refraction at all or TOTAL REFLECTION which means
// there is no refraction.


}



const int maxdepth=3;

vec3 trace(Ray ray) {


}




void main() {
    Ray ray;
    ray.start = cameraPosition;
    ray.dir = normalize(p - cameraPosition);
    fragmentColor = vec4(trace(ray), 1);
}