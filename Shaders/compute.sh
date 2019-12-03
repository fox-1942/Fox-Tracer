#version 430

struct Ray
{
	vec3 origin;
	vec3 direction;
};

struct Sphere
{
	vec3 position;
	float radius;
};

uniform Sphere spheres[3];


float SphereIntersection (in Ray ray, in Sphere sphere)
{
	vec3 distance = ray.origin - sphere.position;
	float b = dot((distance * 2), ray.direction);
	float c = dot(distance, distance) - (sphere.radius * sphere.radius);

	float disc = b * b - 4 * c;
	if (disc < 0)
		return 0;
	else
		disc = sqrt(disc);

	// Always 2 solutions when pulling the square root.
	float result1 = -b + disc;
	float result2 = -b - disc;

	return (result2 > Epsilon) ? result2 / 2 : ((result1 > Epsilon) ? result1 / 2 : 0);
}