#include "physics.h"


#ifndef WORLD_MIN
#define WORLD_MIN vec3(-20.f,-20.f,-20.f)
#define WORLD_MAX vec3( 20.f, 20.f, 20.f)
#endif // !WORLD_MIN



void Physics::step(Scene *scene, float dt, int iterations)
{
    /* Aliases */
    std::vector<vec3> &position = particles.position;
    std::vector<vec3> &pPosition = particles.pPosition;
    std::vector<vec3> &velocity = particles.velocity;
    std::vector<float> &invmass = particles.invmass;
    std::vector<int> &phase = particles.phase;

    int id = performance::startTimer("Physics");

	/* Based on 2007 PBD, NOT Unified Framework */

	const float GRAVITY = 6.0f;
	for (std::vector<glm::vec3>::size_type i = 0; i != particles.cardinality; i++) {
		/*
		* For all particles i
		* Apply forces			v_i = v_i + dt * f_ext(x_i)
		* Damp velocities		-- Skip for now -- TODO --
		* Predict position		x_i^* = x_i + dt * v_i
		*/
		velocity[i] = velocity[i] - glm::vec3(0.f, dt * GRAVITY, 0.f); // Gravity
		pPosition[i] = position[i] + dt * velocity[i]; // symplectic Euler
		// ******************************************************************************************************************

		// For all particles i
		// Generate collision constraints
		/*
		* Add constraints from collisions to already given list of constraints
		*/
		// TODO
		// Temporary collision check and handler for some plane
		/*if (pPosition[i].y < -10) {
			pPosition[i].y = -10;
		}*/
        pPosition[i] = (min)((max)(WORLD_MIN, pPosition[i]), WORLD_MAX);
		// ******************************************************************************************************************
	}

        // Breakable constraints
        constraints.erase(
                std::remove_if(
                    constraints.begin(), 
                    constraints.end(),
                    [](Constraint *c) {
                        //return (c->evaluate() > c->threshold);
                        if (c->evaluate() > c->threshold)
                        {
                            // Minska constraints i c
                            for (unsigned int i = 0; i < c->particles.size(); i++) {
                                c->particles[i]->numBoundConstraints--;
                            }
                            return true;
                        } 
                        else 
                        { 
                            return false; 
                        }
                    }),
                    constraints.end()
                );


	// For all particles i
	// Generate collision constraints
	/* Add constraints from collisions to already given list of constraints */
    for (unsigned int i = 0; i != particles.cardinality; i++)
    {
        // Check collisions with scene
        for (std::vector<Triangle>::iterator t = scene->triangles.begin(); t != scene->triangles.end(); t++)
        {
            Intersection isect;
            if (intersect((*t), particles, i, isect))
            {
                pPosition[i] += isect.responseGradient * (isect.responseDistance * (1 + restitutionCoefficient));
            }
        }

        // Check collisions with other particles
        for (unsigned int j = 0; j < i; j++) 
        {
            Intersection isect;
            if (phase[i] != phase[j] 
				&& intersect(particles, i, j, isect)) 
			{
                pPosition[i] += isect.point;
                pPosition[j] += isect.responseGradient;// * isect.responseDistance;
            }
        }
    }
	/* 
	 * Stationary iterative linear solver - Gauss-Seidel 
	 */

	for (int i = 0; i < iterations; i++)
	{
		for (Constraint *c : constraints)
		{
			if (c->evaluate())
			{ 
				for (std::vector<Particle*>::iterator p = c->particles.begin(); p != c->particles.end(); p++)
				{
					// delta p_i = -w_i * s * grad_{p_i} C(p) * stiffness correction 
                    (*p)->pPos -= (*p)->invmass * c->evaluateScaleFactor() * c->evaluateGradient(p) * (1 - pow(1 - c->stiffness, 1 / (float)i)) * overRelaxConst / (float)(*p)->numBoundConstraints;
                }
			}
		}
	}

	for (std::vector<glm::vec3>::size_type i = 0; i != particles.cardinality; i++) 
	{
		/*
		* For all particles i
		* v_i = (x_i^* - x_i) / dt
		* x_i = x_i^*
		*/
		velocity[i] = (pPosition[i] - position[i]) / dt;
		// rough attempt at particle sleeping implementation in order to make particles stay in one place - most likely needs proper friction to work
	
        if (glm::length(position[i] - pPosition[i]) > pSleeping)
        {
			position[i] = pPosition[i];
        }
		// ***************************************************************************************************************************

		/*
		* Update velocities according to friction and restituition coefficients
		*/
		// Naive friction implementation
		if (position[i].y <= -10) {
			velocity[i].z *= 0.4f;
			velocity[i].x *= 0.4f;
		}
	}
		// Update velocities according to friction and restituition coefficients
		/* Skip this for now */
	
	performance::stopTimer(id);
	
}


Physics::Physics() 
{
    //constraints.reserve(2000000);
}
