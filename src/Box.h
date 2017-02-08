#pragma once

#include <vector>
#include "BoxConfig.h"
#include "Particle.h"

class Box
{
public:
	Box();
	~Box();
	float width, height, depth, mass;
	int num_particles_x, num_particles_y, num_particles_z;

	std::vector<Particle *> particles;
};


Box *make_box(BoxConfig * const config);

