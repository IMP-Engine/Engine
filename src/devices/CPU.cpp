#include "CPU.h"

// TODO there are some small parts which can be parallelized

CPU::CPU(ParticleData & particles, ConstraintData & constraints, std::vector<Triangle> & triangles, bool parallel)
    : Device(particles, constraints, triangles), particleRenderer(), parallel(parallel)
{
    particleRenderer.init();
}

void CPU::verletIntegrate(float dt, glm::vec3 forces)
{
    if (parallel)
    {
        tbb::parallel_for(tbb::blocked_range<size_t>(0, particles.cardinality),
            [&](const tbb::blocked_range<size_t>& r)
        {
            for (size_t i = r.begin(); i != r.end(); i++)
            {
                particles.velocity[i] += dt * forces;
                particles.pPosition[i] = particles.position[i] + dt * particles.velocity[i];
            }
        });
    }
    else 
    {
        for (size_t i = 0; i != particles.cardinality; i++) 
        {
            particles.velocity[i] += dt * forces;
            particles.pPosition[i] = particles.position[i] + dt * particles.velocity[i];
        }
    }
    
}

void CPU::solveConstraintsJacobi(int iterations, float omega)
{
    // Space should already be allocated
    throw std::exception("Not implemented");
}

void CPU::solveConstraintsGaussSeidel(int iterations, float omega)
{
    DistanceConstraintData &distanceConstraints = constraints.distanceConstraints;
    std::vector<glm::vec3> &pPosition = particles.pPosition;
    for (int i = 0; i < iterations; i++)
    {

        /**
        * Distance Constraints
        */
        if (parallel) {
            tbb::parallel_for(
                tbb::blocked_range<size_t>(0, distanceConstraints.cardinality),
                DistanceConstraintData::SolveDistanceConstraint(particles, constraints.distanceConstraints, omega, i));
        }
        else {
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
                        * omega;

                    pPosition[p2] -=
                        delta2
                        * (1 - pow(1 - distanceConstraints.stiffness[constraintIndex], 1 / (float)i))
                        * omega;
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
                pPosition[p] -= delta * omega;
            }
        }

        /**
        * Plane Collision Constraints
        */
        vec3 delta1(0);
        vec3 delta2(0);
        for (int i = 0; i < constraints.planeCollisionConstraints.cardinality; i++)
        {
            if (constraints.planeCollisionConstraints.solvePlaneCollision(delta1, i, particles, false))
            {
                int p = constraints.planeCollisionConstraints.particles[i];
                pPosition[p] -= delta1 * omega;
            }
        }

        // TODO fix
        /**
        * Particle Collision Constraints
        */
        /*for (int i = 0; i < constraints.particleCollisionConstraints.cardinality; i++)
        {
            if (constraints.particleCollisionConstraints.solveDistanceConstraint(delta1, delta2, i, particles, false))
            {
                int p1 = constraints.particleCollisionConstraints.particles[i][0];
                int p2 = constraints.particleCollisionConstraints.particles[i][1];
                pPosition[p1] -= delta1 * omega;
                pPosition[p2] -= delta2 * omega;

                // Friction
                glm::vec3 contactNormal = (pPosition[p1] - pPosition[p2]) / length(pPosition[p1] - pPosition[p2]); //-delta1 / d;
                float d = length(delta1 * omega); // distance along the collision normal that the particles are projected
                glm::vec3 v = (pPosition[p1] - position[p1]) - (pPosition[p2] - position[p2]);
                glm::vec3 tangentialDelta = v - (dot(v, contactNormal) * contactNormal); // [(px[i] - x[i]) - (px[j] - x[j])] tangential to n
                glm::vec3 frictionalPosDelta = (invmass[p1] / (invmass[p1] + invmass[p2])) * tangentialDelta;
                if (length(tangentialDelta) >= (staticFC * d)) // Is particles relative velocity above traction threshold?
                    frictionalPosDelta *= min(kineticFC * d / length(tangentialDelta), 1.f); // if so, apply Columb friction

                pPosition[p1] += frictionalPosDelta;
                pPosition[p2] -= frictionalPosDelta * invmass[p2] / (invmass[p1] + invmass[p2]);
            }
        }*/
    }
}


void CPU::updateParticles(float dt, float pSleeping)
{
    for (std::vector<glm::vec3>::size_type i = 0; i != particles.cardinality; i++)
    {
        particles.velocity[i] = (particles.pPosition[i] - particles.position[i]) / dt;
        if (glm::length(particles.position[i] - particles.pPosition[i]) > pSleeping)
        {
            particles.position[i] = particles.pPosition[i];
        }
    }
}

void CPU::detectCollisionsTriangles()
{
    for (int i = 0; i < constraints.planeCollisionConstraints.cardinality; i++)
    {   // TODO Might want to reconsider where this decreasing + clearing is done to avoid silly check
        if (constraints.planeCollisionConstraints.particles[i] < particles.cardinality)
            particles.numBoundConstraints[constraints.planeCollisionConstraints.particles[i]]--;
    }
    constraints.planeCollisionConstraints.clear();

    if (parallel)
    {
        tbb::parallel_for(
            tbb::blocked_range<size_t>(0, particles.cardinality),
            [&](tbb::blocked_range<size_t>&r)
        {
            for (size_t particleIndex = r.begin(); particleIndex != r.end(); ++particleIndex)
            {
                for (std::vector<Triangle>::iterator t = triangles.begin(); t != triangles.end(); ++t)
                {
                    Intersection isect;
                    if (intersect((*t), particles, particleIndex, isect))
                    {
                        PlaneCollisionConstraint c;
                        c.particleIndex = particleIndex;
                        c.normal = isect.responseGradient;
                        c.distance = glm::dot(t->v0, isect.responseGradient) + particles.radius[particleIndex];
                        particles.numBoundConstraints[particleIndex]++;

                        constraints.planeCollisionConstraints.addConstraint(c);
                    }
                }
            }
        });
    }
    else
    {
        for (unsigned int i = 0; i != particles.cardinality; i++)
        {
            for (std::vector<Triangle>::iterator t = triangles.begin(); t != triangles.end(); t++)
            {
                Intersection isect;
                if (intersect((*t), particles, i, isect))
                {
                    PlaneCollisionConstraint c;
                    c.particleIndex = i;
                    c.normal = isect.responseGradient;
                    c.distance = glm::dot(t->v0, isect.responseGradient) + particles.radius[i];
                    particles.numBoundConstraints[i]++;

                    constraints.planeCollisionConstraints.addConstraint(c);
                }
            }
        }
    }
}

void CPU::detectCollisionParticles()
{
    for (int i = 0; i < constraints.particleCollisionConstraints.cardinality; i++)
    {   // TODO Might want to reconsider where this decreasing + clearing is done to avoid silly check
        if (constraints.particleCollisionConstraints.particles[i][0] < particles.cardinality)
            particles.numBoundConstraints[constraints.particleCollisionConstraints.particles[i][0]]--;
        if (constraints.particleCollisionConstraints.particles[i][1] < particles.cardinality)
            particles.numBoundConstraints[constraints.particleCollisionConstraints.particles[i][1]]--;
    }

    constraints.particleCollisionConstraints.clear();

    collision::createCollisionConstraints(particles, constraints.particleCollisionConstraints);
}

// TODO parallelize
void CPU::stabilizationPass(int stabilizationIterations)
{
    vec3 delta1(0);
    vec3 delta2(0);
    for (int j = 0; j < stabilizationIterations; j++)
    {
        for (int i = 0; i < constraints.planeCollisionConstraints.cardinality; i++)
        {
            if (constraints.planeCollisionConstraints.solvePlaneCollision(delta1, i, particles, true))
            {
                int p = constraints.planeCollisionConstraints.particles[i];
                particles.position[p] -= delta1;
                particles.pPosition[p] -= delta1;
            }
        }

        for (int i = 0; i < constraints.particleCollisionConstraints.cardinality; i++)
        {
            if (constraints.particleCollisionConstraints.solveDistanceConstraint(delta1, delta2, i, particles, true))
            {
                int p1 = constraints.particleCollisionConstraints.particles[i][0];
                int p2 = constraints.particleCollisionConstraints.particles[i][1];
                particles.position[p1] -= delta1;
                particles.pPosition[p1] -= delta1;
                particles.position[p2] -= delta2;
                particles.pPosition[p2] -= delta2;
            }
        }
    }
}

void CPU::dampCollision(float restitutionCoefficientT, float restitutionCoefficientN)
{
    for (unsigned int i = 0; i < constraints.planeCollisionConstraints.particles.size(); i++)
    {
        // Split into tangential and normal velocity
        vec3 nVel = glm::dot(constraints.planeCollisionConstraints.normals[i], particles.velocity[constraints.planeCollisionConstraints.particles[i]]) * constraints.planeCollisionConstraints.normals[i];
        vec3 tVel = -nVel + particles.velocity[constraints.planeCollisionConstraints.particles[i]];
        // Damp tangential velocity according to parameter
        tVel *= restitutionCoefficientT;
        // flip normal velocity
        nVel = glm::dot(nVel, constraints.planeCollisionConstraints.normals[i]) > 0 ? nVel : -nVel;
        // Damp normal velocity according to parameter
        nVel *= restitutionCoefficientN;
        // Combine
        particles.velocity[constraints.planeCollisionConstraints.particles[i]] = tVel + nVel;
    }
}

void CPU::renderParticles(glm::mat4 & modelViewProjectionMatrix, glm::mat4 & modelViewMatrix, glm::vec3 & viewSpaceLightPosition, glm::mat4 & projectionMatrix, int height)
{
    particleRenderer.render(particles, modelViewProjectionMatrix, modelViewMatrix, viewSpaceLightPosition, projectionMatrix, height);
}

void CPU::stop(ParticleData & particles, ConstraintData & constraints)
{
}

void CPU::start(ParticleData & particles, ConstraintData & constraints)
{
}



