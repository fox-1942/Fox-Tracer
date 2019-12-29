#version 460 core
precision mediump float;

struct Ray {
    vec3 start;
    vec3 dir;
};

struct Material {
    vec3 ka, kd, ks;
    float  shininess;
};

struct Light {
    vec3 direction;
    vec3 ambient;
};

struct Hit {
    float t;
    vec3 position;
    vec3 normal;
    float material;
};

struct Sphere {
    vec3 center;
    float radius;
};

struct Light{
    vec3 direction;
    vec3 ambient;
};

uniform vec3 lightAmbient;
uniform vec3 cameraPosition;
uniform int numberOfObjects;
uniform Sphere objects[300];
uniform sampler2D texture_diffuse1;

uniform Light light;

float Theta1;
float Theta2;
float cosTheta1;
float cosTheta2;

in  vec3 p;
in vec2 v_TexCoord;
out vec4 fragmentColor;
vec3 N;

float angleDegCalculator(vec3 first, vec3 second){
    float dotProduct = dot(first, second);

    float firstLength = length(first);
    float secondLength = length(second);

    float angle = acos(dotProduct / firstLength * secondLength);
    return angle;
}

float fresnelCalculator(float fractionIndexOfMedium1, float fractionIndexOfMedium2, vec3 I, vec3 N){
    cosTheta1=cos(Theta1);
    Theta1=angleDegCalculator(I, N);

    // In this case, we change the two fraction indices, because the ray is coming from inside.
    // if cos is negative the angle is more than 90 degrees, so we are under the x-axis
    if (cosTheta1 < 0) {
        float temp;
        temp = fractionIndexOfMedium1;
        fractionIndexOfMedium1 = fractionIndexOfMedium2;
        fractionIndexOfMedium2 = temp;
    }

    // If a sine of angle is more than 1, the degree of the angle is more than 90 degrees, so there is no
    // refracted part. Only reflected.
    if (sin(Theta2)>=1.0f){
        // Total reflection happens
        float fresnelReflected=1;
        return fresnelReflected;
    }

    else {
        // Cases when there is refraction and reflaction as well, not only reflaction.
        // Fresnel equation can tell us how much is the reflected part and we can calculate the refracted part of light as well.
        // At Fresnel equation we calculate the longitudinal ans transversal part of reflected light.

        // Based on Snell's law, we calculate Theta2.
        Theta2=sin(Theta1) * fractionIndexOfMedium1/fractionIndexOfMedium2;

        cosTheta2=cos(Theta2);

        float fresnelReflectedLongitudinal=pow(((fractionIndexOfMedium2 * cosTheta1) - (fractionIndexOfMedium1 * cosTheta2)) / ((fractionIndexOfMedium2 * cosTheta1) + (fractionIndexOfMedium1 * cosTheta2)), 2);

        float fresnelReflectedTransversal=pow(((fractionIndexOfMedium1 * cosTheta1) - (fractionIndexOfMedium2 *cosTheta2)) / ((fractionIndexOfMedium1 * cosTheta1) + (fractionIndexOfMedium2 * cosTheta2)), 2);

        float fresnelReflected=0.5*(fresnelReflectedLongitudinal+fresnelReflectedTransversal);

        // float fresnelRefractedComponent= 1 - fresnelReflected;

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

    Sphere selected;

    for (int i = 0; i < numberOfObjects; i++) {

        Hit hit = intersect(objects[i], ray);//  hit.t < 0 if no intersection

        if (hit.t > 0 && (bestHit.t < 0 || hit.t < bestHit.t))
        bestHit = hit;
        selected=objects[i];

    }
    N=bestHit.position-selected.center;

    return bestHit;
}


// Vector of the reflected light
vec3 reflectedRayDir(const vec3 I, const vec3 N){
    vec3 result= I - 2 * dot(I, N) * N;
    return result;

    // GLSL has reflect() method built-in as well, which is the same.
}


vec3 refractedRayDir(const vec3 I, const vec3 N, const float fractionIndexOfMedium2)
{
    vec3 refractedRayDir;
    float fractionIndexOfMedium1=1;
    float fractionIndexOfMediumSecond=fractionIndexOfMedium2;// Why cannot read it from the parameter?
    vec3 n=N;

    if (cosTheta1 < 0) { // Is it correct? Shouldn't I change cos to negative as well?
        float temp;
        temp = fractionIndexOfMedium1;
        fractionIndexOfMedium1 = fractionIndexOfMediumSecond;
        fractionIndexOfMediumSecond = temp;
        n=-N;
    }

    float ratioOfIndicesOfRefraction = fractionIndexOfMediumSecond / fractionIndexOfMedium1;

    refractedRayDir = refract(I, N, ratioOfIndicesOfRefraction);
    return refractedRayDir;


    /* k = 1.0 - eta * eta * (1.0 - dot(N, I) * dot(N, I));
     if (k < 0.0)
         R = genType(0.0);       // or genDType(0.0)
     else
         R = eta * I - (eta * dot(N, I) + sqrt(k)) * N;

     if (cosi < 0) { cosi = -cosi; } else { std::swap(etai, etat); n= -N; }
     float eta = etai / etat;
     float k = 1 - eta * eta * (1 - cosi * cosi);
     return k < 0 ? 0 : eta * I + (eta * cosi - sqrtf(k)) * n;

     */
}


vec3 trace(Ray ray) {
    int maxIterationNumber=0;
    vec3 Radiance= vec3(0, 0, 0);

    float fractionIndexOfMedium1=1.0f;
    float fractionIndexOfMedium2=1.5f;

    Hit hit=firstIntersect(ray);

    // in case there is no object occluding the ray
    if (hit.t==-1){
        return light.ambient;
    }

    // we are supposing rough material only with no reflaction,
    // raycasting only with some spheres

    //Computing DirectLight

    vec3 ka=(0.3f, 0.2f, 0.1f)*M_PI;
    Radiance=light.ambient*ka;

    //Computing Shadow
    const float epsilon = 0.0001f;

    Ray shadowRay;
    shadowRay.start = hit.position + hit.normal * epsilon;
    shadowRay.dir = light.direction;

    Hit shadowHit=firstIntersect(shadowRay);

    if(shadowHit.t<0){
        Radiance+=
    }



    return Radiance;

}


/* while(maxIterationNumber<4){

     Hit hit=firstIntersect(ray);

     // in case there is no object occluding the ray
     if(hit.t==-1){
         Radiance=+lightAmbient;
     }

     float reflectedRatio=fresnelCalculator(fractionIndexOfMedium1, fractionIndexOfMedium2,  ray.dir, N);

     // in case there is total reflaction
     if (reflectedRatio>=1){
         Ray shadowRay;
         shadowRay.start = hit.position;  // + EPS
         shadowRay.dir = reflectedRayDir(ray.dir, N);

         Radiance=

         maxIterationNumber++;
         trace(ray);
     }


     // in case there is refractive and reflactive part is well
     if(reflectedRatio<1){

         float reflectedRatio=fresnelCalculator(fractionIndexOfMedium1, fractionIndexOfMedium2,  ray.dir, N);


         //Creating shadow-ray
         Ray shadowRay;
         shadowRay.start=hit.position;
         shadowRay.dir=


         trace(ray);
         maxIterationNumber++;

     }
     vec3 radiance;
     return radiance;
 }
 */


bool shadowRayIntersectExaminer(Ray shadowRay){
    for (int i=0;i<numberOfObjects;i++){
        if (intersect(shadowRay==-1)){
            return false;
        }

        else {
            return true;
        }
    }
    return false;
}



void main() {
    Ray ray;
    ray.start = cameraPosition;
    ray.dir = normalize(p - cameraPosition);
    // fragmentColor=texture(texture_diffuse1, v_TexCoord);
    fragmentColor = vec4(trace(ray), 1);
}

