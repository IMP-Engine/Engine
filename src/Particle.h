#pragma once

#include <glm/vec3.hpp>

using namespace glm;

struct Particle
{
	vec3 pos;
	vec3 velocity;
	float invmass;
	float phase;
};

