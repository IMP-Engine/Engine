#include "Triangle.h"
#include <iostream>

using namespace glm;

bool intersect(vec3 v0, vec3 v1, vec3 v2, vec3 center, float radius, Intersection &isect) {
    vec3 A = v0 - center;
    vec3 B = v1 - center;
    vec3 C = v2 - center;
    float rr = radius * radius;
    vec3 normal = cross(B - A, C - A);
    float d = dot(A, normal);
    float e = dot(normal, normal);

    if (d * d > rr * e) {
        return false;
    }


    float aa = dot(A, A);
    float ab = dot(A, B);
    float ac = dot(A, C);
    float bb = dot(B, B);
    float bc = dot(B, C);
    float cc = dot(C, C);

    if ((aa > rr) & (ab > aa) & (ac > aa)) 
    { 
        return false; 
    }

    if ((bb > rr) & (ab > bb) & (bc > bb)) 
    { 
        return false; 
    }

    if ((cc > rr) & (ac > cc) & (bc > cc)) 
    { 
        return false; 
    }

    vec3 AB = B - A;
    vec3 BC = C - B;
    vec3 CA = A - C;
    float d1 = ab - aa;
    float d2 = bc - bb;
    float d3 = ac - cc;
    float e1 = dot(AB, AB);
    float e2 = dot(BC, BC);
    float e3 = dot(CA, CA);
    vec3 Q1 = A * e1 - d1 * AB;
    vec3 Q2 = B * e2 - d2 * BC;
    vec3 Q3 = C * e3 - d3 * CA;
    vec3 QC = C * e1 - Q1;
    vec3 QA = A * e2 - Q2;
    vec3 QB = B * e3 - Q3;
    if (dot(Q1, Q1) > rr * e1 * e1 && dot(Q1, QC) > 0) {
        return false;
    }
    if (dot(Q2, Q2) > rr * e2 * e2 && dot(Q2, QA) > 0) {
        return false;
    }
    if (dot(Q3, Q3) > rr * e3 * e3 && dot(Q3, QB) > 0) {
        return false;
    }

    // Response

    vec3 normNormal = normal / sqrt(e);
    float distance = dot(A, normNormal);

    if (d < 0) {
        isect.response = normNormal * (radius - distance);
    }
    else {
        isect.response = normNormal * distance;
    }

    return true;
}