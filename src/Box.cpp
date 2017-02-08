#include "Box.h"



Box::Box()
{
}


Box::~Box()
{
}



Box *make_box(BoxConfig * const config) {
	Box *box = new Box;
	int num_particles = config->num_particles_x * config->num_particles_y * config->num_particles_z;

	//box->particles.resize(num_particles);

	float dx = config->width  / (config->num_particles_x-1);
	float dy = config->height / (config->num_particles_y-1);
	float dz = config->depth  / (config->num_particles_z-1);
	 
	box->width = config->width;
	box->height = config->height;
	box->depth = config->depth;
	box->mass = config->mass;
	box->num_particles_x = config->num_particles_x;
	box->num_particles_y = config->num_particles_y;
	box->num_particles_z = config->num_particles_z;

	for (int i = 0; i < config->num_particles_x; i++) {
		for (int j = 0; j < config->num_particles_y; j++) {
			for (int k = 0; k < config->num_particles_z; k++) {
				Particle *p = new Particle;
				p->pos = make_vector<float>(i*dx, j*dy, k*dz);
				p->mass = config->mass / num_particles;
				p->velocity = make_vector(0.f, 0.f, 0.f);
				p->force = make_vector(0.f, 0.f, 0.f);
				box->particles.push_back(p);
			}
		}
	}
	return box;
}