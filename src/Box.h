#pragma once

#include <vector>
#include "BoxConfig.h"
#include "Particle.h"

class Box
{
public:
	Box();
	~Box();
	float mass;
	float3 dimensions, center_pos;
	int3 num_particles;

	std::vector<Particle *> particles;
};


Box *make_box(BoxConfig * const config);

