#pragma once

#include <glm/vec3.hpp>

using namespace glm;

#define PARTICLE_RADIUS 0.1f
#define PARTICLE_RADIUS_2 0.2f

struct Particle
{
	vec3 pos;
	vec3 pPos;
	vec3 velocity;
	float invmass;
	int phase;

	bool operator==(const Particle& p) {
		return p.pos == this->pos && p.velocity == this->velocity && p.invmass == this->invmass && p.phase == this->phase;
	}
};

