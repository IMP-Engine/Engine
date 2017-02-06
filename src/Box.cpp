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

	box->particles = new Particle[num_particles];

	for (int i = 0; i < config->num_particles_x; i++)
	{
		
	}
}