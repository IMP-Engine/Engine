
#include "../primitives.h"

#include "glm/glm.hpp"
#include "glm/vec3.hpp"

#ifndef INTERSECTION_STRUCT
struct Intersection {
    glm::vec3 point;
    glm::vec3 response;
};
#endif // !INTERSECTION


bool intersect(Triangle &triangle, Particle &p, float radius, Intersection &isect);