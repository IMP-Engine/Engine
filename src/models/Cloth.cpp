#include "Cloth.h"

void model::makeClothModel(ModelConfig & config, bool hasFixedCorners, ParticleData &particles, ConstraintData &constraints)
{
    unsigned int start = particles.cardinality;

    float dx = config.scale.x / (config.numParticles.x - 1);
    float dz = config.scale.z / (config.numParticles.z - 1);
    float radius = min(dx,dz) / 2;
    glm::vec3 base = config.centerPos - (config.scale / 2.f);

    for (unsigned int i = 0;i < config.numParticles.x;i++) 
    {
        for (unsigned int j = 0; j < config.numParticles.z; j++)
        {
            Particle p;
            p.invmass = config.invmass;
            p.radius = radius;
            p.phase = config.phase;
            p.velocity = glm::vec3(0);

            p.pos = glm::vec3(
                base.x + dx*i,
                base.y,
                base.z + dz*j
            );

            p.pPos = p.pos;

            addParticle(p, particles);
            
            if (i > 0) {
                DistanceConstraint c;
                c.distance = dx;
                c.firstParticleIndex = start + i*config.numParticles.z + j;
                c.secondParticleIndex = start + (i-1)*config.numParticles.z + j;
                c.threshold = config.distanceThreshold;
                c.equality = false;
                c.stiffness = config.stiffness;

                addConstraint(constraints.distanceConstraints, c);
            }

            if (j > 0) {
                DistanceConstraint c;
                c.distance = dx;
                c.firstParticleIndex = start + i*config.numParticles.z + j;
                c.secondParticleIndex = start + i*config.numParticles.z + (j-1);
                c.threshold = config.distanceThreshold;
                c.equality = false;
                c.stiffness = config.stiffness;

                addConstraint(constraints.distanceConstraints, c);
            }
        }
    }


    
}
