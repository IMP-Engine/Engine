
#include "../primitives.h"

#include "glm/glm.hpp"
#include "glm/vec3.hpp"

#ifndef INTERSECTION_STRUCT
struct Intersection {
    glm::vec3 point;
    glm::vec3 response;
};
#endif // !INTERSECTION


bool intersect(vec3 v0, vec3 v1, vec3 v2, vec3 center, float radius, Intersection &isect);