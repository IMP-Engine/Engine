#include "physics.h"
#include <glm/vec3.hpp>
#include "Constraint.h"

namespace physics {

void simulate(std::vector<Particle*>* particles, std::vector<Constraint*>* constraints , float dt)
{
	std::vector<glm::vec3> pPos(particles->size());
	// Based on 2007 PBD, NOT Unified Framework 

	// For all particles i
	// Apply forces			v_i = v_i + dt * f_ext(x_i)
	// Damp velocities		-- Skip for now --
	// Predict position		x_i^* = x_i + dt * v_i

	for (std::vector<glm::vec3>::size_type i = 0; i != particles->size(); i++) {
		(*particles)[i]->velocity = (*particles)[i]->velocity - glm::vec3(0.5f); // Gravity (Placeholder value. Also, using vec3 instead of float3.)
		pPos[i] = (*particles)[i]->pos + dt * (*particles)[i]->velocity;
	}



	// For all particles i
	// Generate collision constraints
	/* Add constraints from collisions to already given list of constraints */




	/* Stationary iterative linear solver */

	for (size_t i = 0; i < 10; i++)
	{
		for (Constraint* c : *constraints)
		{
			if (c->evaluate())
			{ 
				for (std::vector<Particle*>::iterator p = c->particles.begin(); p != c->particles.end(); p++)
				{
					// delta p_i = w_i * s * grad_{p_i} C(p) * stiffness correction 
					(*p)->pos -= (*p)->invmass * c->evaluateScaleFactor() * c->evaluateGradient(p) * (1 - pow(1 - c->stiffness, 1/(float)i));
				}
			}
		}
	}


	// For all particles i
	// v_i = (x_i^* - x_i) / dt
	// x_i = x_i^*

	for (std::vector<glm::vec3>::size_type i = 0; i != particles->size(); i++) 
	{
		(*particles)[i]->velocity = (pPos[i] - (*particles)[i]->pos) / dt;
		(*particles)[i]->pos = pPos[i];
	}


		// Update velocities according to friction and restituition coefficients
		/* Skip this for now */
}

			
}