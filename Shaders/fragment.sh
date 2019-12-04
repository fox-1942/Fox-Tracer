#version 460 core

struct Ray {
    vec3 start
    vec3 dir;
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

Hit intersect(const Sphere object, const Ray ray) {
		Hit hit;
		hit.t = -1;                             // we first suppose there is no intersection
		vec3 dist = ray.start - object.center;  // distance from center to the start of ray

		float a = dot(ray.dir, ray.dir);
		float b = dot(dist, ray.dir) * 2.0f;
		float c = dot(dist, dist) - object.radius * object.radius;

		float discr = b * b - 4.0f * a * c;   // discriminant

		if (discr < 0) // There is no intersection
		  return hit;

		float sqrt_discr = sqrt(discr);
		float t1 = (-b + sqrt_discr) / 2.0f / a;
		float t2 = (-b - sqrt_discr) / 2.0f / a;

		if (t1 <= 0)
		  return hit;  // There is no intersection or just touching the surface

		hit.t = (t2 > 0) ? t2 : t1;
		hit.position = ray.start + ray.dir * hit.t;
		hit.normal = (hit.position - object.center) / object.radius;

		return hit;
	  }


Hit firstIntersect(Ray ray) {
		Hit bestHit;
		bestHit.t = -1;

		for (int i = 0; i < numberOfObjects; i++) {

			Hit hit = intersect(objects[i], ray); //  hit.t < 0 if no intersection

		    if (hit.t > 0 && (bestHit.t < 0 || hit.t < bestHit.t))
		      bestHit = hit;
		}

		return bestHit;
}

vec3 trace(Ray ray) {

}

void main() {
		Ray ray;
		ray.start = cameraPosition;
		ray.dir = normalize(p - cameraPosition);
		fragmentColor = vec4(trace(ray), 1);
}