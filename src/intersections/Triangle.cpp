#include "Triangle.h"
#include <iostream>

using namespace glm;

bool intersect(Triangle &triangle, Particle &p, float radius, Intersection &isect) {

    // Check intersection of target position and triangle

    // Translate problem so that the particle is at origo
    vec3 A = triangle.v0 - triangle.v1;
    vec3 B = triangle.v0 - triangle.v2;
    vec3 C = triangle.v1 - triangle.v2;
    vec3 CA = C - A;
    vec3 BA = B - A;
    /*
     * Step 1: Check if particle intesect with plane.
     */
    float rr = radius * radius;
    float dist = dot(A, triangle.normal);

    // Always end early.
    if (dist * dist > rr) {
        return false;
    }


    float aa = dot(A, A);

    /**
     * Step 2: check if any of the triangle is inside the sphere.
     */

    // First vertex
    if (aa < rr) {
        float distance = dot(A, triangle.normal);
        isect.response = triangle.normal * (radius - distance);

        return true;
    }

    // Second vertex
    float bb = dot(B, B);
    if (bb < rr) {
        float distance = dot(A, triangle.normal);
        isect.response = triangle.normal * (radius - distance);

        return true;
    }

    // Third vertex
    float cc = dot(C, C);
    if (cc < rr) {

        float distance = dot(A, triangle.normal);
        isect.response = triangle.normal * (radius - distance);

        return true;
    }
    

    /**
     * Step 3: Check if projected center of particle is inside triangle
     */
    float distance = dot(A, triangle.normal);
    vec3 projetedPosition = p.pPos - distance * triangle.normal;

    // Baycentric check

    // Compute vectors        
    vec3 pPrim = projetedPosition - A; // particle position relative to point A

    // Compute dot products
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
    if ((u >= 0) && (v >= 0) && (u + v < 1)) {

        isect.response = triangle.normal * (radius - distance);
        return true;
    }


    /**
     * Step 4: Check if any edge intersect the sphere
     **/

    float t = (dot(CA, p.pPos) - dot(CA, A)) / dotCACA;
    vec3 Q = p.pPos - A + t*CA;
    float d2 = dot(Q,Q);

    if (d2 == rr && (t >= 0 || t <= 1)) {
        isect.response = vec3(0);
        return true;
    }

    t = (dot(BA, p.pPos) - dot(BA, A)) / dotBABA;
    Q = p.pPos - A+t*BA;
    d2 = dot(Q, Q);

    if (d2 < rr && (t >= 0 || t <= 1)) {
        isect.response = sqrt(d2) * normalize(Q - p.pPos);
        return true;
    }

    vec3 CB = C - B;
    t = (dot(CB, p.pPos) - dot(CB, A)) / dot(CB,CB);
    Q = p.pPos - B + t*CB;
    d2 = dot(Q, Q);

    if (d2 < rr && (t >= 0 || t <= 1)) {
        isect.response = sqrt(d2) * normalize(Q - p.pPos);
        return true;
    }

    return false;
}