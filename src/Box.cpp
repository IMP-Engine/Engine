#include "Box.h"



Box::Box()
{
}


Box::~Box()
{
}



Box *make_box(BoxConfig * const config) {
	Box *box = new Box;
	int num_particles = config->num_particles.x * config->num_particles.y * config->num_particles.z;

	//box->particles.resize(num_particles);

	float dx = config->dimensions.x / (config->num_particles.x-1);
	float dy = config->dimensions.y / (config->num_particles.y-1);
	float dz = config->dimensions.z / (config->num_particles.z-1);
	 

	// Get the "minimum" corner to use as base position
	float3 base_pos = config->center_pos - (config->dimensions * 0.5f);

	box->dimensions = config->dimensions;
	box->center_pos = config->center_pos;
	box->mass = config->mass;
	box->num_particles = config->num_particles;

	for (int i = 0; i < config->num_particles.x; i++) {
		for (int j = 0; j < config->num_particles.y; j++) {
			for (int k = 0; k < config->num_particles.z; k++) {
				Particle *p = new Particle;

				// Offset from base
				p->pos = base_pos + make_vector<float>(i*dx, j*dy, k*dz);

				// Uniform distribution of mass
				p->mass = config->mass / num_particles;

				p->velocity = make_vector(0.f, 0.f, 0.f);
				p->force = make_vector(0.f, 0.f, 0.f);

				box->particles.push_back(p);
			}
		}
	}
	return box;
}