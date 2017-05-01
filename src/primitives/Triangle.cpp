#include "Triangle.h"



Triangle::Triangle()
{
}


Triangle::~Triangle()
{
}


void Triangle::aabb(vec3 &minP, vec3 &maxP) {
    minP = min(v0, min(v1, v2));
    maxP = max(v0, max(v1, v2));
}


vec3 Triangle::center() {
    return vec3(
        (v0.x + v1.x + v2.x) / 3.f,
        (v0.y + v1.y + v2.y) / 3.f,
        (v0.z + v1.z + v2.z) / 3.f
    );
}