#include "Triangle.h"
#include <iostream>

using namespace glm;

bool intersect(vec3 v0, vec3 v1, vec3 v2, Particle p, float radius, Intersection &isect) {

    // Check intersection of target position and triangle

    // Translate problem so that the particle is at origo
    vec3 A = v0 - p.pPos;
    vec3 B = v1 - p.pPos;
    vec3 C = v2 - p.pPos;

    /*
     * Step 1: Check if particle intesect with plane.
     */
    float rr = radius * radius;
    vec3 normal = cross(B - A, C - A);
    float dist = dot(A, normal);
    float e = dot(normal, normal);
    normal /= sqrt(e);
    // Always end early.
    if (dist * dist > rr * e) {
        return false;
    }


    float aa = dot(A, A);

    /**
     * Step 2: check if any of the triangle is inside the sphere.
     */

    // First vertex
    if (aa < rr) {
        float distance = dot(A, normal);
        isect.response = normal * (radius - distance);

        return true;
    }

    // Second vertex
    float bb = dot(B, B);
    if (bb < rr) {
        float distance = dot(A, normal);
        isect.response = normal * (radius - distance);

        return true;
    }

    // Third vertex
    float cc = dot(C, C);
    if (cc < rr) {

        float distance = dot(A, normNormal);
        isect.response = normNormal * (radius - distance);

        return true;
    }
    


    return true;
}