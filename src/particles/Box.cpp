#include "Box.h"
#include "../DistanceConstraint.h"


Box::Box()
{
}


Box::~Box()
{
	for (auto c : this->constraints) {
		delete c;
	}
}



Box *make_box(BoxConfig * const config) {
	Box *box = new Box;
	int num_particles = config->num_particles.x * config->num_particles.y * config->num_particles.z;

	//box->particles.resize(num_particles);

	float dx = config->dimensions.x / (config->num_particles.x-1);
	float dy = config->dimensions.y / (config->num_particles.y-1);
	float dz = config->dimensions.z / (config->num_particles.z-1);
	 

	// Get the "minimum" corner to use as base position
	vec3 base_pos = config->center_pos - (config->dimensions * 0.5f);

	box->dimensions = config->dimensions;
	box->center_pos = config->center_pos;
	box->mass = config->mass;
	box->num_particles = config->num_particles;

	for (int i = 0; i < config->num_particles.x; i++) {
		for (int j = 0; j < config->num_particles.y; j++) {
			for (int k = 0; k < config->num_particles.z; k++) {
				Particle p = Particle();

				// Offset from base
				p.pos = base_pos + vec3(i*dx, j*dy, k*dz);

				// Uniform distribution of mass
				p.invmass = config->mass / num_particles;
				p.velocity = vec3(0.f, 0.f, 0.f);
				p.phase = config->phase;

				box->particles.push_back(p);
			}
		}
	}
	// Do two passes? One binding the y planes together, One binding the x planes together?

	float stiffness = 0.1;
	for (int i = 0; i < box->particles.size(); i++) {
		for (int j = 0; j < box->particles.size(); j++) {
			if (j != i && glm::distance(box->particles[i].pos, box->particles[j].pos) < 0.1+sqrt(dx*dx + dy*dy + dz*dz))
			{
				Constraint* c = new DistanceConstraint(
					&box->particles[i],
					&box->particles[j],
					stiffness, glm::distance(box->particles[i].pos, box->particles[j].pos));
				box->constraints.push_back(c);
			}
		}
	}
	
	return box;
}