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
    std::vector<tbb::atomic<int>>   &numBoundConstraints = particles.numBoundConstraints;

    int id = performance::startTimer("Solve constraints");
    resolveConstraints(position, pPosition, invmass, numBoundConstraints, constraints.planeCollisionConstraints, constraints.particleCollisionConstraints);
    performance::stopTimer(id);

    for (std::vector<glm::vec3>::size_type i = 0; i != particles.cardinality; i++) 
    {
        /*
         * For all particles i
         * v_i = (x_i^* - x_i) / dt
         * x_i = x_i^*
         */
        if (glm::length(position[i] - pPosition[i]) > pSleeping)
        {
            position[i] = pPosition[i];
        }
    }
}

static int iter = 0;

void Physics::resolveConstraints(std::vector<glm::vec3> & position, std::vector<glm::vec3> & pPosition, std::vector<float> & invmass, std::vector<tbb::atomic<int>> & numBoundConstraints, PlaneCollisionConstraintData & planeConstraints, DistanceConstraintData & particleConstraints)
{
    DistanceConstraintData &distanceConstraints = constraints.distanceConstraints;
    for (int i = 1; i < iterations+1; i++)
    {
        float sumC = 0;
        ++iter;

        for (int constraintIndex = 0; constraintIndex < distanceConstraints.cardinality; constraintIndex++)
        {
            sumC += std::abs(distanceConstraints.evaluate(constraintIndex, particles));
        }
        // print time and residual
        printf("%f\t%f\n", glfwGetTime(),  sumC);
        // print iteration and residual
        printf("%i\t%f\n", iter, sumC);
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
                        * overRelaxConst;

                    pPosition[p2] -= 
                        delta2 
                        * (1 - pow(1 - distanceConstraints.stiffness[constraintIndex], 1 / (float)i))
                        * overRelaxConst;
                }
            }
        }
        

    }
}

void Physics::dampPlaneCollision(std::vector<tbb::atomic<int>> & numBoundConstraints, std::vector<glm::vec3> & velocity, PlaneCollisionConstraintData & planeConstraints)
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

void Physics::detectCollisions(Scene * scene, std::vector<tbb::atomic<int>> & numBoundConstraints, PlaneCollisionConstraintData & planeConstraints, std::vector<int> & phase, std::vector<glm::vec3> & pPosition)
{
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
