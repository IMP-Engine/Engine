#include "physics.h"
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include "Constraint.h"

namespace physics {

void simulate(std::vector<Particle>* particles, std::vector<Constraint*>* constraints , float dt, int iterations)
{
	// Based on 2007 PBD, NOT Unified Framework 

	// For all particles i
	// Apply forces			v_i = v_i + dt * f_ext(x_i)
	// Damp velocities		-- Skip for now --
	// Predict position		x_i^* = x_i + dt * v_i

	for (std::vector<glm::vec3>::size_type i = 0; i != particles->size(); i++) {
		(*particles)[i].velocity = (*particles)[i].velocity - glm::vec3(0.f, 0.2f, 0.f) * (*particles)[i].invmass; // Gravity (Placeholder value. Also, using vec3 instead of float3.)
		(*particles)[i].pPos = (*particles)[i].pos + dt * (*particles)[i].velocity;
	}



	// For all particles i
	// Generate collision constraints
	/* Add constraints from collisions to already given list of constraints */
	for (std::vector<glm::vec3>::size_type i = 0; i != particles->size(); i++) {
		if ((*particles)[i].pPos.y < -10) {
			(*particles)[i].pPos.y = -10;
		}
	}

	printf("Num %d", constraints->size());
	/* Stationary iterative linear solver */

	for (size_t i = 0; i < iterations; i++)
	{
		for (Constraint* c : *constraints)
		{
			if (c->evaluate())
			{ 
				for (std::vector<Particle*>::iterator p = c->particles.begin(); p != c->particles.end(); p++)
				{
					// delta p_i = -w_i * s * grad_{p_i} C(p) * stiffness correction 
			
					(*p)->pPos -= (*p)->invmass * c->evaluateScaleFactor() * c->evaluateGradient(p) * (1 - pow(1 - c->stiffness, 1/(float)i));
				}
			}
		}
	}


	// For all particles i
	// v_i = (x_i^* - x_i) / dt
	// x_i = x_i^*

	for (std::vector<glm::vec3>::size_type i = 0; i != particles->size(); i++) 
	{
		(*particles)[i].velocity = ((*particles)[i].pPos - (*particles)[i].pos) / dt;
		if (glm::length((*particles)[i].pos - (*particles)[i].pPos) > 0.01)
			(*particles)[i].pos = (*particles)[i].pPos;
	}


	for (std::vector<glm::vec3>::size_type i = 0; i != particles->size(); i++) {
		if ((*particles)[i].pos.y <= -10) {
			(*particles)[i].velocity.z *= 0.4;
			(*particles)[i].velocity.x *= 0.4;
		}
	}
		// Update velocities according to friction and restituition coefficients
		/* Skip this for now */
}

			
}