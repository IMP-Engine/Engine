#include "bruteforce.h"

void bruteForce(ParticleData & particles, DistanceConstraintData & constraints, bool ignorePhase)
{
    
    // Check collisions with other particles
    for (unsigned int i = 0; i < particles.cardinality; i++) {
        for (unsigned int j = 0; j < i; j++)
        {
            Intersection isect;
            if ((ignorePhase || particles.phase[i] != particles.phase[j])
                && intersect(particles, i, j, isect))
            {
                DistanceConstraint c;
                c.firstParticleIndex = i;
                c.secondParticleIndex = j;
                c.equality = false;
                c.distance = particles.radius[i] + particles.radius[j];
                particles.numBoundConstraints[i]++;
                particles.numBoundConstraints[j]++;

                addConstraint(constraints, c);
            }
        }
    }
    
}
