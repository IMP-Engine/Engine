#include "physics.h"

#ifndef WORLD_MIN
#define WORLD_MIN vec3(-20.f,-20.f,-20.f)
#define WORLD_MAX vec3( 20.f, 20.f, 20.f)
#endif // !WORLD_MIN

#define WIND 4.f

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
		velocity[i] -= glm::vec3(0.f, dt * GRAVITY, 0.f); // Gravity

        if (scene->windActive) {
            velocity[i] += glm::vec3(dt * WIND, 0.f, 0.f);
        }

		pPosition[i] = position[i] + dt * velocity[i]; // symplectic Euler
		// ******************************************************************************************************************
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
    
    id = performance::startTimer("Scene collision detection");
    detectCollisions(scene, numBoundConstraints, constraints.planeCollisionConstraints, phase, pPosition);
    performance::stopTimer(id);

    id = performance::startTimer("Detect collisions");
    collision::createCollisionConstraints(particles, constraints.particleCollisionConstraints);
    performance::stopTimer(id);

    id = performance::startTimer("Solve collisions");
    resolveCollisions(position, pPosition, invmass, constraints.planeCollisionConstraints, constraints.particleCollisionConstraints);
    performance::stopTimer(id);

    id = performance::startTimer("Solve constraints");
    resolveConstraints(position, pPosition, invmass, numBoundConstraints, constraints.planeCollisionConstraints, constraints.particleCollisionConstraints);
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
    // Update velocities according to friction and restitution coefficients
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

void Physics::resolveConstraints(std::vector<glm::vec3> & position, std::vector<glm::vec3> & pPosition, std::vector<float> & invmass, std::vector<int> & numBoundConstraints, PlaneCollisionConstraintData & planeConstraints, DistanceConstraintData & particleConstraints)
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
                if (distanceConstraints.solveDistanceConstraint(delta1, delta2, constraintIndex, particles, false))
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
        
        /**
        * Fixed Position Constraints
        */
        FixedPointConstraintData &fixedPosConstraints = constraints.fixedPointConstraints;
        vec3 delta(0);

        for (unsigned int constraintIndex = 0; constraintIndex < fixedPosConstraints.cardinality; constraintIndex++)
        {
            if (fixedPosConstraints.solveDistanceConstraint(constraintIndex, delta, particles))
            {
                // delta p_i = -w_i * s * grad_{p_i} C(p) * stiffness correction
                int p = fixedPosConstraints.particle[constraintIndex];
                pPosition[p] -= delta * overRelaxConst / (float)numBoundConstraints[p];
            }
        }

        /**
        * Plane Collision Constraints
        */
        vec3 delta1(0);
        vec3 delta2(0);
        for (int i = 0; i < planeConstraints.cardinality; i++)
        {
            if (planeConstraints.solvePlaneCollision(delta1, i, particles, false))
            {
                int p = planeConstraints.particles[i];
                pPosition[p] -= delta1 * overRelaxConst;
            }
        }

        /**
        * Particle Collision Constraints
        */
        for (int i = 0; i < particleConstraints.cardinality; i++)
        {
            if (particleConstraints.solveDistanceConstraint(delta1, delta2, i, particles, false))
            {
                int p1 = particleConstraints.particles[i][0];
                int p2 = particleConstraints.particles[i][1];
                pPosition[p1] -= delta1 * overRelaxConst;
                pPosition[p2] -= delta2 * overRelaxConst;

                // Friction
                glm::vec3 contactNormal = (pPosition[p1] - pPosition[p2]) / length(pPosition[p1] - pPosition[p2]); //-delta1 / d;
                float d = length(delta1 * overRelaxConst); // distance along the collision normal that the particles are projected
                glm::vec3 v = (pPosition[p1] - position[p1]) - (pPosition[p2] - position[p2]);
                glm::vec3 tangentialDelta = v - (dot(v, contactNormal) * contactNormal); // [(px[i] - x[i]) - (px[j] - x[j])] tangential to n
                glm::vec3 frictionalPosDelta = (invmass[p1] / (invmass[p1] + invmass[p2])) * tangentialDelta;
                if (length(tangentialDelta) >= (staticFC * d)) // Is particles relative velocity above traction threshold?
                    frictionalPosDelta *= min(kineticFC * d / length(tangentialDelta), 1); // if so, apply Columb friction

                pPosition[p1] += frictionalPosDelta;
                pPosition[p2] -= frictionalPosDelta * invmass[p2] / (invmass[p1] + invmass[p2]);
            }
        }
    }
}

void Physics::dampPlaneCollision(std::vector<int> & numBoundConstraints, std::vector<glm::vec3> & velocity, PlaneCollisionConstraintData & planeConstraints)
{
    for (unsigned int i = 0; i < planeConstraints.particles.size(); i++)
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

void Physics::resolveCollisions(std::vector<glm::vec3> & position, std::vector<glm::vec3> & pPosition, std::vector<glm::float32> & invmass, PlaneCollisionConstraintData & planeConstraints, DistanceConstraintData & particleConstraints)
{
    vec3 delta1(0);
    vec3 delta2(0);
    for (int j = 0; j < stabilizationIterations; j++)
    {
        for (int i = 0; i < planeConstraints.cardinality; i++)
        {
            if (planeConstraints.solvePlaneCollision(delta1, i, particles, true))
            {
                int p = planeConstraints.particles[i];
                position[p] -= delta1 * overRelaxConst;
                pPosition[p] -= delta1 * overRelaxConst;
            }
        }

        for (int i = 0; i < particleConstraints.cardinality; i++)
        {
            if (particleConstraints.solveDistanceConstraint(delta1, delta2, i, particles, true))
            {
                int p1 = particleConstraints.particles[i][0];
                int p2 = particleConstraints.particles[i][1];
                position[p1]  -= delta1 * overRelaxConst;
                pPosition[p1] -= delta1 * overRelaxConst;
                position[p2]  -= delta2 * overRelaxConst;
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
        tbb::parallel_for(
            tbb::blocked_range<size_t>(0, particles.cardinality),
            PlaneCollisionConstraintData::PlaneCollisionDetection(scene, particles, planeConstraints));
    }
    else
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

                    planeConstraints.addConstraint(c);
                }
            }
        }
    }
}
