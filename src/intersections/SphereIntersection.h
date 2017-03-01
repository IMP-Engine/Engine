#pragma once

#include "../primitives.h"

#ifndef INTERSECTION_STRUCT
struct Intersection {
    glm::vec3 point;
    glm::vec3 response;
};
#endif // !INTERSECTION_STRUCT

bool intersect(vec3 &c1, float invMass1, vec3 &c2, float invMass2, Intersection &i);

