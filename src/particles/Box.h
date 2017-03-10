#pragma once

#include <vector>
#include "BoxConfig.h"
#include "Particle.h"
#include "../constraints/Constraint.h"

class Box
{
public:
	Box();
	~Box();
	float mass;
	vec3 dimensions, center_pos;
	ivec3 numParticles;

	std::vector<Particle*> particles;
	std::vector<Constraint*> constraints;
};


Box *make_box(BoxConfig *const config, std::vector<Particle> &particleStorage);

