#include "Triangle.h"
#include <iostream>

using namespace glm;

bool intersect(Triangle &triangle, ParticleData particles, int index, Intersection &isect) {

    /*
        Aliases
    */
    vec3  &pPos   = particles.pPosition[index];
    float &radius = particles.radius[index];

    // Check intersection of target position and triangle

    /*
     * Step 1: Check if particle intesect with plane.
     */
    float rr = radius * radius;
    float dist = dot(pPos-triangle.v0, triangle.normal);

    // Always end early.
    if (dist * dist > rr) {
        return false;
    }


	isect.responseGradient = triangle.normal;
	isect.responseDistance = (radius - dist);


    vec3 A = triangle.v1 - triangle.v0;
    vec3 B = triangle.v2 - triangle.v0;
    vec3 C = triangle.v2 - triangle.v1;
    /**
     * Step 2: check if any of the triangle vertexes is inside the sphere.
     */

    // First vertex
	float aa = dot(triangle.v0 - pPos, triangle.v0 - pPos);
    if (aa < rr) 
	{
        return true;
    }

    // Second vertex
	float bb = dot(triangle.v1 - pPos, triangle.v1 - pPos);
    if (bb < rr) 
	{
        return true;
    }

    // Third vertex
	float cc = dot(triangle.v2 - pPos, triangle.v2 - pPos);
    if (cc < rr) 
	{
        return true;
    }
    

    /**
     * Step 3: Check if projected center of particle is inside triangle
     */
    vec3 projetedPosition = pPos - dist * triangle.normal;

    // Baycentric check
    
	// particle position relative to v0
    vec3 pPrim = projetedPosition - triangle.v0; 

    // Compute dot products
    vec3 CA = triangle.v2 - triangle.v0;
    vec3 BA = triangle.v1 - triangle.v0;
    float dotCACA = dot(CA, CA);
    float dotCABA = dot(CA, BA);
    float dotBABA = dot(BA, BA);
    float dotCApPrim = dot(CA, pPrim);
    float dotBApPrim = dot(BA, pPrim);

    // Compute barycentric coordinates
    float invDenom = 1 / (dotCACA * dotBABA - dotCABA * dotCABA);
    float u = (dotBABA * dotCApPrim - dotCABA * dotBApPrim) * invDenom;
    float v = (dotCACA * dotBApPrim - dotCABA * dotCApPrim) * invDenom;

    // Check if point is in triangle
    if ((u >= 0) && (v >= 0) && (u + v < 1)) 
	{
        return true;
    }


    /**
     * Step 4: Check if any edge intersect the sphere
     **/


	// First edge, v0 to v1
    float t = (dot(A, pPos) - dot(A, triangle.v0)) / dot(A,A);
    vec3 Q = triangle.v0 + t*A;
	vec3 Qp = pPos - Q;
    float d2 = dot(Qp,Qp);

    if (d2 < rr && (t >= 0 || t <= 1)) {
        return true;
    }

    t = (dot(B, pPos) - dot(B, triangle.v0)) / dot(B,B);
    Q = triangle.v0 + t*B;
	Qp = pPos - Q;
    d2 = dot(Qp, Qp);

    if (d2 < rr && (t >= 0 || t <= 1)) {
        return true;
    }

    t = (dot(C, pPos) - dot(C, triangle.v1)) / dot(C,C);
    Q = triangle.v1 + t*C;
	Qp = pPos - Q;
    d2 = dot(Qp, Qp);

    if (d2 < rr && (t >= 0 || t <= 1)) {
        return true;
    }

    return false;
}