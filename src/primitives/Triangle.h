#pragma once

#include "glm/glm.hpp"
#include "glm/vec3.hpp"

using namespace glm;

class Triangle
{
public:
	Triangle();
	~Triangle();
	vec3 v0;
	vec3 v1;
	vec3 v2;
	void aabb(vec3 &min, vec3 &max);
	vec3 center();
};

