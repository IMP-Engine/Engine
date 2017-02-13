#include "physics.h"


namespace physics {

void simulate(std::vector<Particle*>* particles/*, std::vector<Constraint*>* constraints */, float dt)
{
	std::vector<float3> pPos(particles->size());
	// Based on 2007 PBD, NOT Unified Framework 

	// For all particles i
	// Apply forces			v_i = v_i + dt * f_ext(x_i)
	// Damp velocities		-- Skip for now --
	// Predict position		x_i^* = x_i + dt * v_i

	for (std::vector<float3>::size_type i = 0; i != particles->size(); i++) {
		//(*particles)[i]->velocity = (*particles)[i]->velocity - vec3(0.5f); // Gravity (Placeholder value. Also, using vec3 instead of float3.)
		pPos[i] = (*particles)[i]->pos + dt * (*particles)[i]->velocity;
	}



	// For all particles i
	// Generate collision constraints
	/* Add constraints from collisions to already given list of constraints */



	// loop until satisfied
	// Project constraints (C1,...,CM, x_1^*,...x_n^*)
	/* Stationary iterative linear solver */



	// For all particles i
	// v_i = (x_i^* - x_i) / dt
	// x_i = x_i^*

	for (std::vector<float3>::size_type i = 0; i != particles->size(); i++) 
	{
		(*particles)[i]->velocity = (pPos[i] - (*particles)[i]->pos) / dt;
		(*particles)[i]->pos = pPos[i];
	}


		// Update velocities according to friction and restituition coefficients
		/* Skip this for now */
}

			
}