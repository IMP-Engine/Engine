#include "physics.h"

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
        // ********************************************************************************
        /*
         * Clamp positions so that we do not lose any particles
         */
        pPosition[i] = (min)((max)(WORLD_MIN, pPosition[i]), WORLD_MAX);
        // ********************************************************************************
    }

    // Breakable constraints
    int id = performance::startTimer("Remove broken constraints");
    constraints.removeBroken(particles);
    performance::stopTimer(id);

    detectCollisions(scene, numBoundConstraints, constraints.planeCollisionConstraints, phase, pPosition);

    id = performance::startTimer("Solve collisions");
    resolveCollisions(position, pPosition, constraints.planeCollisionConstraints, constraints.particleCollisionConstraints);
    performance::stopTimer(id);

    id = performance::startTimer("Solve constraints");
    resolveConstraints(pPosition, invmass, numBoundConstraints);
    performance::stopTimer(id);

    for (std::vector<glm::vec3>::size_type i = 0; i != particles.cardinality; i++) 
    {
        /*
         * For all particles i
         * v_i = (x_i^* - x_i) / dt
         * x_i = x_i^*
         */
        velocity[i] = (pPosition[i] - position[i]) / dt;	
        if (glm::length(position[i] - pPosition[i]) > pSleeping)
        {
            position[i] = pPosition[i];
        }
    }

    id = performance::startTimer("Damp collisions");
    // Update velocities according to friction and restituition coefficients
    dampPlaneCollision(numBoundConstraints, velocity, constraints.planeCollisionConstraints);
    performance::stopTimer(id);

    // Collision constraints can unfortunately only exist for the limited time of a single step
    for (int i = 0; i < constraints.planeCollisionConstraints.cardinality; i++) 
    { 
        numBoundConstraints[constraints.planeCollisionConstraints.particles[i]]--;
    }
    for (int i = 0; i < constraints.particleCollisionConstraints.cardinality; i++)
    { 
        numBoundConstraints[constraints.particleCollisionConstraints.particles[i][0]]--;
        numBoundConstraints[constraints.particleCollisionConstraints.particles[i][1]]--;
    }
    constraints.planeCollisionConstraints.clear();
    constraints.particleCollisionConstraints.clear();

}

void Physics::resolveConstraints(std::vector<glm::vec3> & pPosition, std::vector<float> & invmass, std::vector<int> & numBoundConstraints)
{
    DistanceConstraintData &distanceConstraints = constraints.distanceConstraints;
    for (int i = 0; i < iterations; i++)
    {

        /**
         * Distance Constraints
         */
        if (parallelConstraintSolve) {
            tbb::parallel_for(
                    tbb::blocked_range<size_t>(0, distanceConstraints.cardinality), 
                    DistanceConstraintData::SolveDistanceConstraint(particles, constraints.distanceConstraints, overRelaxConst, i));
        } else {
            vec3 delta1(0);
            vec3 delta2(0);
            for (int constraintIndex = 0; constraintIndex < distanceConstraints.cardinality; constraintIndex++)
            {
                if (distanceConstraints.solveDistanceConstraint(delta1, delta2, constraintIndex, particles))
                {
                    // delta p_i = -w_i * s * grad_{p_i} C(p) * stiffness correction 
                    ivec2 &constraintParticles = distanceConstraints.particles[constraintIndex];
                    int p1 = constraintParticles[0];
                    int p2 = constraintParticles[1];
                    pPosition[p1] -= 
                        delta1
                        * (1 - pow(1 - distanceConstraints.stiffness[constraintIndex], 1 / (float)i))
                        * overRelaxConst
                        / (float)numBoundConstraints[p1];

                    pPosition[p2] -= 
                        delta2 
                        * (1 - pow(1 - distanceConstraints.stiffness[constraintIndex], 1 / (float)i))
                        * overRelaxConst
                        / (float)numBoundConstraints[p2];
                }
            }
        }
    }
}

void Physics::dampPlaneCollision(std::vector<int> & numBoundConstraints, std::vector<glm::vec3> & velocity, PlaneCollisionConstraintData & planeConstraints)
{
    for (int i = 0; i < planeConstraints.particles.size(); i++)
    {
        // Split into tangential and normal velocity
        vec3 nVel = dot(planeConstraints.normals[i], velocity[planeConstraints.particles[i]]) * planeConstraints.normals[i];
        vec3 tVel = -nVel + velocity[planeConstraints.particles[i]];
        // Damp tangential velocity according to parameter
        tVel *= restitutionCoefficientT;
        // flip normal velocity
        nVel = dot(nVel, planeConstraints.normals[i]) > 0 ? nVel : -nVel;
        // Damp normal velocity according to parameter
        nVel *= restitutionCoefficientN;
        // Combine
        velocity[planeConstraints.particles[i]] = tVel + nVel;
    }
}

void Physics::resolveCollisions(std::vector<glm::vec3> & position, std::vector<glm::vec3> & pPosition, PlaneCollisionConstraintData & planeConstraints, DistanceConstraintData & particleConstraints)
{
    vec3 delta1(0);
    vec3 delta2(0);
    for (int j = 0; j < collisionIterations; j++)
    {
        for (int i = 0; i < planeConstraints.cardinality; i++) {
            if (planeConstraints.solvePlaneCollision(delta1, i, particles))
            {
                int p = planeConstraints.particles[i];
                position[p] -= delta1 * overRelaxConst;
                pPosition[p] -= delta1 * overRelaxConst;
            }
        }

        for (int i = 0; i < particleConstraints.cardinality; i++)
        {
            if (particleConstraints.solveDistanceConstraint(delta1, delta2, i, particles))
            {
                int p1 = particleConstraints.particles[i][0];
                int p2 = particleConstraints.particles[i][1];
                pPosition[p1] -= delta1 * overRelaxConst;
                pPosition[p2] -= delta2 * overRelaxConst;
            }
        }
    }
}

void Physics::detectCollisions(Scene * scene, std::vector<int> & numBoundConstraints, PlaneCollisionConstraintData & planeConstraints, std::vector<int> & phase, std::vector<glm::vec3> & pPosition)
{
    int id;
    if (parallelDetectCollisions)
    {
        id = performance::startTimer("Detect scene collisions");

        tbb::parallel_for(
            tbb::blocked_range<size_t>(0, particles.cardinality),
            PlaneCollisionConstraintData::PlaneCollisionDetection(scene, particles, planeConstraints));

        performance::stopTimer(id);
    }
    else
    {
        int id = performance::startTimer("Detect scene collisions");
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

                    planeConstraints.addConstraint(c);
                }
            }
        }
        performance::stopTimer(id);
    }

    id = performance::startTimer("Detect particle collisions");
    for (unsigned int i = 0; i != particles.cardinality; i++)
    {
        // Check collisions with other particles
        for (unsigned int j = 0; j < i; j++)
        {
            Intersection isect;
            if (phase[i] != phase[j]
                && intersect(particles, i, j, isect))
            {
                DistanceConstraint c;
                c.firstParticleIndex = i;
                c.secondParticleIndex = j;
                c.equality = false;
                c.distance = particles.radius[i] + particles.radius[j];
                numBoundConstraints[i]++;
                numBoundConstraints[j]++;

                addConstraint(constraints.particleCollisionConstraints, c);
            }
        }
    }
    performance::stopTimer(id);
}
