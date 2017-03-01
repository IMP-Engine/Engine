#pragma once


#include "glm/glm.hpp"
#include "glm/vec3.hpp"

#define INTERSECTION_STRUCT 1
struct Intersection {
    glm::vec3 point;
    glm::vec3 response;
};


#include "intersections/Triangle.h"
#include "intersections/SphereIntersection.h"