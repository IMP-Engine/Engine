#include "physics.h"
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <stdio.h>
#include "Constraint.h"

namespace physics {

void simulate(std::vector<Particle>* particles, std::vector<Constraint*>* constraints , float dt, int iterations)
{
	/* Based on 2007 PBD, NOT Unified Framework */

	const float GRAVITY = 4.0f;
	for (std::vector<glm::vec3>::size_type i = 0; i != particles->size(); i++) {
		/*
		* For all particles i
		* Apply forces			v_i = v_i + dt * f_ext(x_i)
		* Damp velocities		-- Skip for now -- TODO --
		* Predict position		x_i^* = x_i + dt * v_i
		*/
		(*particles)[i].velocity = (*particles)[i].velocity - glm::vec3(0.f, dt * GRAVITY, 0.f) * (*particles)[i].invmass; // Gravity
		(*particles)[i].pPos = (*particles)[i].pos + dt * (*particles)[i].velocity; // symplectic Euler 
		// ******************************************************************************************************************

		// For all particles i
		// Generate collision constraints
		/*
		* Add constraints from collisions to already given list of constraints
		*/
		// TODO
		// Temporary collision check and handler for some plane
		if ((*particles)[i].pPos.y < -10) {
			(*particles)[i].pPos.y = -10;
		}
		// ******************************************************************************************************************
	}

	/* 
	 * Stationary iterative linear solver - Gauss-Seidel 
	 */
	for (size_t i = 0; i < iterations; i++)
	{
		for (Constraint* c : *constraints)
		{
			if (c->equality)
			{ 
				for (std::vector<Particle*>::iterator p = c->particles.begin(); p != c->particles.end(); p++)
				{
					// delta p_i = -w_i * s * grad_{p_i} C(p) * stiffness correction 
					(*p)->pPos -= (*p)->invmass * c->evaluateScaleFactor() * c->evaluateGradient(p) * (1 - pow(1 - c->stiffness, 1/(float)i));
				}
			}
		}
	}

	
	for (std::vector<glm::vec3>::size_type i = 0; i != particles->size(); i++) 
	{
		/*
		* For all particles i
		* v_i = (x_i^* - x_i) / dt
		* x_i = x_i^*
		*/
		(*particles)[i].velocity = ((*particles)[i].pPos - (*particles)[i].pos) / dt;
		// rough attempt at particle sleeping implementation in order to make particles stay in one place - most likely needs proper friction to work
		if (glm::length((*particles)[i].pos - (*particles)[i].pPos) > 0.01)
			(*particles)[i].pos = (*particles)[i].pPos;
		// ***************************************************************************************************************************

		/*
		* Update velocities according to friction and restituition coefficients
		*/
		// Naive friction implementation
		if ((*particles)[i].pos.y <= -10) {
			(*particles)[i].velocity.z *= 0.4;
			(*particles)[i].velocity.x *= 0.4;
		}
	}
}

			
}
