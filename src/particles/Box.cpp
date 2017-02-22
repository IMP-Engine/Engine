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
	float stiffness = 1;
	float distance = 1;
	for (int i = 1; i < config->num_particles.x-1; i++) {
		for (int j = 1; j < config->num_particles.y-1; j++) {
			for (int k = 1; k < config->num_particles.z-1; k++) {
				DistanceConstraint* c1 = new DistanceConstraint(
					&box->particles[i * config->num_particles.z * config->num_particles.y + j * config->num_particles.z + k],
					&box->particles[i * config->num_particles.z * config->num_particles.y + j * config->num_particles.z + k - 1],
					stiffness, distance, true);
				DistanceConstraint* c2 = new DistanceConstraint(
					&box->particles[i * config->num_particles.z * config->num_particles.y + j * config->num_particles.z + k],
					&box->particles[i * config->num_particles.z * config->num_particles.y + j * config->num_particles.z + k + 1],
					stiffness, distance, true);
				DistanceConstraint* c3 = new DistanceConstraint(
					&box->particles[i * config->num_particles.z * config->num_particles.y + j * config->num_particles.z + k],
					&box->particles[i * config->num_particles.z * config->num_particles.y + (j-1) * config->num_particles.z + k],
					stiffness, distance, true);
				DistanceConstraint* c4 = new DistanceConstraint(
					&box->particles[i * config->num_particles.z * config->num_particles.y + j * config->num_particles.z + k],
					&box->particles[i * config->num_particles.z * config->num_particles.y + (j+1) * config->num_particles.z + k],
					stiffness, distance, true);
				DistanceConstraint* c5 = new DistanceConstraint(
					&box->particles[i * config->num_particles.z * config->num_particles.y + j * config->num_particles.z + k],
					&box->particles[(i+1) * config->num_particles.z * config->num_particles.y + j * config->num_particles.z + k],
					stiffness, distance, true);
				DistanceConstraint* c6 = new DistanceConstraint(
					&box->particles[i * config->num_particles.z * config->num_particles.y + j * config->num_particles.z + k],
					&box->particles[(i-1) * config->num_particles.z * config->num_particles.y + j * config->num_particles.z + k],
					stiffness, distance, true);
				box->constraints.push_back(c1);
				box->constraints.push_back(c2);
				box->constraints.push_back(c3);
				box->constraints.push_back(c4);
				box->constraints.push_back(c5);
				box->constraints.push_back(c6);
				
			}
		}
	}
	return box;
}