#pragma once
#include <glm/vec3.hpp>

using namespace glm;

struct BoxConfig {
	float mass;
	vec3 center_pos, dimensions;
	ivec3 numParticles;
	int phase;
	float stiffness;
	float distanceThreshold;
};
