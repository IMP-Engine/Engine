#include "Cloth.h"

void model::makeClothModel(ModelConfig & config, bool hasFixedCorners, ParticleData &particles, ConstraintData &constraints)
{
    unsigned int start = particles.cardinality;

    float dx = config.scale.x / (config.numParticles.x - 1);
    float dz = config.scale.z / (config.numParticles.z - 1);
    float radius = min(dx,dz) / 2;
    glm::vec3 base = config.centerPos - (config.scale / 2.f);

    for (int i = 0;i < config.numParticles.x;i++) 
    {
        for (int j = 0; j < config.numParticles.z; j++)
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
            
            DistanceConstraint c;
            c.threshold = config.distanceThreshold;
            c.equality = true;
            c.stiffness = config.stiffness;

            if (i > 0) {
                c.distance = dx;
                c.firstParticleIndex = start + i*config.numParticles.z + j;
                c.secondParticleIndex = start + (i-1)*config.numParticles.z + j;

                addConstraint(constraints.distanceConstraints, c);
                particles.numBoundConstraints[c.firstParticleIndex]++;
                particles.numBoundConstraints[c.secondParticleIndex]++;
            }

            if (j > 0) {
                c.distance = dx;
                c.firstParticleIndex = start + i*config.numParticles.z + j;
                c.secondParticleIndex = start + i*config.numParticles.z + (j-1);
                
                addConstraint(constraints.distanceConstraints, c);
                particles.numBoundConstraints[c.firstParticleIndex]++;
                particles.numBoundConstraints[c.secondParticleIndex]++;
            }


            c.stiffness = config.stiffness * 0.01f;

            if (j > 0 && i > 0) {
                c.distance = sqrt(dx*dx + dz*dz);
                c.firstParticleIndex = start + i*config.numParticles.z + j;
                c.secondParticleIndex = start + (i - 1)*config.numParticles.z + (j - 1);

                addConstraint(constraints.distanceConstraints, c);
                particles.numBoundConstraints[c.firstParticleIndex]++;
                particles.numBoundConstraints[c.secondParticleIndex]++;
            }


            if (j < config.numParticles.z-1 && i > 0) {
                c.distance = sqrt(dx*dx + dz*dz);
                c.firstParticleIndex = start + i*config.numParticles.z + j;
                c.secondParticleIndex = start + (i - 1)*config.numParticles.z + (j + 1);

                addConstraint(constraints.distanceConstraints, c);
                particles.numBoundConstraints[c.firstParticleIndex]++;
                particles.numBoundConstraints[c.secondParticleIndex]++;
            }
        }
    }

    if (hasFixedCorners) {
        FixedPointConstraint c;
        c.particle = start;
        c.position = particles.position[start];
        addConstraint(constraints.fixedPointConstraints, c);


        c.particle = start+ config.numParticles.x-1;
        c.position = particles.position[c.particle];
        addConstraint(constraints.fixedPointConstraints, c);
    }

    for (int i = 0; i < particles.position.size(); i++) {
    particles.position[i] *= 4;
    particles.pPosition[i] *= 4;
}
    
}
