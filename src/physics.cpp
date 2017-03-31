#include "physics.h"

#include <tuple>

#ifndef WORLD_MIN
#define WORLD_MIN vec3(-20.f,-20.f,-20.f)
#define WORLD_MAX vec3( 20.f, 20.f, 20.f)
#endif // !WORLD_MIN



void Physics::step(Scene *scene, float dt)
{

    /* Aliases */
    std::vector<vec3>  &position  = particles.position;
    std::vector<vec3>  &pPosition = particles.pPosition;
    std::vector<vec3>  &velocity  = particles.velocity;
    std::vector<float> &invmass   = particles.invmass;
    std::vector<int>   &phase     = particles.phase;
    std::vector<int>   &numBoundConstraints = particles.numBoundConstraints;

    int id = performance::startTimer("Physics");

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
		/*
		* Clamp positions so that we do not lose any particles
		*/
        pPosition[i] = (min)((max)(WORLD_MIN, pPosition[i]), WORLD_MAX);
		// ******************************************************************************************************************
	}

	// Breakable constraints
	constraints.removeBroken(particles);

    detectCollisions(scene, numBoundConstraints, constraints.triangleCollisionConstraints, phase, pPosition);

    resolveCollisons(position, pPosition, constraints.triangleCollisionConstraints);
	
    resolveConstraints(pPosition, invmass, numBoundConstraints);

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
	}
	
    // Update velocities according to friction and restituition coefficients
    dampPlaneCollision(numBoundConstraints, velocity, constraints.triangleCollisionConstraints);

	// Collision constraints can unfortunately only exist for the limited time of a single step
    constraints.triangleCollisionConstraints.clear();

	performance::stopTimer(id);
	
}

void Physics::resolveConstraints(std::vector<glm::vec3> & pPosition, std::vector<float> & invmass, std::vector<int> & numBoundConstraints)
{
    for (int i = 0; i < iterations; i++)
    {

        /**
        * Distance Constraints
        */
        DistanceConstraintData &distanceConstraints = constraints.distanceConstraints;
        for (int constraintIndex = 0; constraintIndex < distanceConstraints.cardinality; constraintIndex++)
        {

            if (distanceConstraints.evaluate(constraintIndex, particles))
            {
                ivec2 &constraintParticles = distanceConstraints.particles[constraintIndex];
                for (int pIndex = 0; pIndex < 2; pIndex++)
                {
                    int p = constraintParticles[pIndex];
                    // delta p_i = -w_i * s * grad_{p_i} C(p) * stiffness correction 
                    pPosition[p] -=
                        invmass[p]
                        * distanceConstraints.scaleFactor(constraintIndex, particles)
                        * distanceConstraints.gradient(constraintIndex, p, particles)
                        * (1 - pow(1 - distanceConstraints.stiffness[constraintIndex], 1 / (float)i))
                        * overRelaxConst
                        / (float)numBoundConstraints[p];
                }
            }
        }
        
        /**
        *
        */
    }
}

void Physics::dampPlaneCollision(std::vector<int> & numBoundConstraints, std::vector<glm::vec3> & velocity, PlaneCollisionConstraintData & planeConstraints)
{
    for (int i = 0; i < planeConstraints.particles.size(); i++)
    {
        numBoundConstraints[planeConstraints.particles[i]]--;
        // Split into tangential and normal velocity
        vec3 nVel = dot(planeConstraints.normals[i], velocity[planeConstraints.particles[i]]) * planeConstraints.normals[i];
        vec3 tVel = -nVel + velocity[planeConstraints.particles[i]];
        // Damp tangential according to parameter
        tVel *= restitutionCoefficient;
        // flip normal velocity
        nVel = dot(nVel, planeConstraints.normals[i]) > 0 ? nVel : -nVel;;
        // Combine
        velocity[planeConstraints.particles[i]] = tVel + nVel;
    }
}

void Physics::resolveCollisons(std::vector<glm::vec3> & position, std::vector<glm::vec3> & pPosition, PlaneCollisionConstraintData & planeConstraints)
{
    vec3 delta(0);
    for (int j = 0; j < collisionIterations; j++)
    {
        for (int i = 0; i < planeConstraints.cardinality; i++) {
            if (planeConstraints.solvePlaneCollision(delta, i, particles))
            {
                int p = planeConstraints.particles[i];
                position[p] -= delta;
                pPosition[p] -= delta;
            }
        }
    }
}

void Physics::detectCollisions(Scene * scene, std::vector<int> & numBoundConstraints, PlaneCollisionConstraintData & planeConstraints, std::vector<int> & phase, std::vector<glm::vec3> & pPosition)
{
    for (unsigned int i = 0; i != particles.cardinality; i++)
    {
        // Check collisions with scene
        for (std::vector<Triangle>::iterator t = scene->triangles.begin(); t != scene->triangles.end(); t++)
        {
            Intersection isect;
            if (intersect((*t), particles, i, isect))
            {
                PlaneCollisionConstraint c;
                c.particleIndex = i;
                c.normal = isect.responseGradient;
                c.distance = glm::dot(t->v0, isect.responseGradient) + particles.radius[i];
                numBoundConstraints[i]++;

                addConstraint(planeConstraints, c);
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
}
