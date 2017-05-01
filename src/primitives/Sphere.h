#pragma once

#include "glm/glm.hpp"
#include "glm/vec3.hpp"

using namespace glm;

class Sphere
{
public:
    Sphere();
    ~Sphere();
    vec3 center;
    float radius;
    void aabb(vec3 &min, vec3 &max);
};

