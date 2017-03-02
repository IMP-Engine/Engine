#pragma once
#include <glm/vec3.hpp>

using namespace glm;

struct BoxConfig {
	float mass;
	vec3 center_pos, dimensions;
	vec3 num_particles;
	int phase;
	float stiffness;
};
