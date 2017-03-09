#pragma once

#include "glm/glm.hpp"
#include "glm/vec3.hpp"

using namespace glm;

class Triangle
{
public:
	Triangle();
	~Triangle();
	vec3 v0, v1, v2;
    vec3 u, v;
    vec3 normal;
	void aabb(vec3 &min, vec3 &max);
	vec3 center();
};

