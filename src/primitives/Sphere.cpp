#include "Sphere.h"



Sphere::Sphere()
{
}


Sphere::~Sphere()
{
}


void Sphere::aabb(vec3 &minP, vec3 &maxP) {
	minP = center - vec3(radius);
	maxP = center + vec3(radius);
}
