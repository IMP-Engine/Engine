#include "physics.h"
#include <glm/vec3.hpp>
#include <glm/glm.hpp>

#ifndef WORLD_MIN
#define WORLD_MIN vec3(-20.f,-20.f,-20.f)
#define WORLD_MAX vec3( 20.f, 20.f, 20.f)
#endif // !WORLD_MIN


namespace physics {

void simulate(std::vector<Particle> *particles/*, std::vector<Constraint*>* constraints */,Scene *scene, float dt)
{
	std::vector<glm::vec3> pPos(particles->size());
	// Based on 2007 PBD, NOT Unified Framework 
    
	// For all particles i
	// Apply forces			v_i = v_i + dt * f_ext(x_i)
	// Damp velocities		-- Skip for now --
	// Predict position		x_i^* = x_i + dt * v_i

	for (std::vector<glm::vec3>::size_type i = 0; i != particles->size(); i++) {
        // Gravity (Placeholder value. Also, using vec3 instead of float3.)
		(*particles)[i].velocity = (*particles)[i].velocity - glm::vec3(0.f, 0.01f, 0.f); 
		pPos[i] = (*particles)[i].pos + dt * (*particles)[i].velocity;
	}




	// For all particles i
	// Generate collision constraints
	/* Add constraints from collisions to already given list of constraints */
    for (unsigned int i = 1; i != particles->size(); i++)
    {

        for (unsigned int t = 0; t < scene->indices.size(); t+=3) 
        {

            vec3 v0 = vec3(scene->vertexes[3 * scene->indices[t]],
                           scene->vertexes[3 * scene->indices[t] + 1],
                           scene->vertexes[3 * scene->indices[t] + 2]);

            vec3 v1 = vec3(scene->vertexes[3 * scene->indices[t+1]],
                           scene->vertexes[3 * scene->indices[t+1] + 1],
                           scene->vertexes[3 * scene->indices[t+1] + 2]);

            vec3 v2 = vec3(scene->vertexes[3 * scene->indices[t+2]],
                           scene->vertexes[3 * scene->indices[t+2] + 1],
                           scene->vertexes[3 * scene->indices[t+2] + 2]);

            Intersection isect;
            if (intersect(v0, v1, v2, pPos[i], PARTICLE_RADIUS,isect)) 
            {
                pPos[i] += isect.response;
            }
        }

        for (unsigned int j = 0; j < i; j++) 
        {
            Intersection isect;
            
            if (intersect(
                pPos[i], (*particles)[i].invmass,
                pPos[j], (*particles)[j].invmass,
                isect)
            ) {
                pPos[i] += isect.point;
                pPos[j] += isect.response;
            }
        }
    }


	// loop until satisfied
	// Project constraints (C1,...,CM, x_1^*,...x_n^*)
	/* Stationary iterative linear solver */

	for (std::vector<glm::vec3>::size_type i = 0; i != particles->size(); i++)
	{
		pPos[i] = min(max(WORLD_MIN, pPos[i]), WORLD_MAX);
	}



	// For all particles i
	// v_i = (x_i^* - x_i) / dt
	// x_i = x_i^*

	for (std::vector<glm::vec3>::size_type i = 0; i != particles->size(); i++) 
	{
		(*particles)[i].velocity = (pPos[i] - (*particles)[i].pos) / dt;
		(*particles)[i].pos = pPos[i];
	}


		// Update velocities according to friction and restituition coefficients
		/* Skip this for now */
	
	
}

			
}