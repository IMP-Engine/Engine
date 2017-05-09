#include "Box.h"


void Box::makeBox(ParticleData &particles, ConstraintData &constraints, model::ModelConfig config) {
 
    unsigned int start = particles.cardinality;

    float dx = config.scale.x, dy = config.scale.y, dz = config.scale.z;

    if (config.numParticles.x > 1) {
        dx /= (config.numParticles.x-1);
    }
    if (config.numParticles.y > 1) {
        dy /= (config.numParticles.y-1);
    }
    if (config.numParticles.z > 1) {
        dz /= (config.numParticles.z-1);
    }

    // Get the "minimum" corner to use as base position
    vec3 base_pos = config.centerPos - (config.scale * 0.5f);
    if (config.numParticles.y == 1) {
        dy = 0;       
    }

    // Create particles
    for (int i = 0; i < config.numParticles.x; i++) {
        for (int j = 0; j < config.numParticles.y; j++) {
            for (int k = 0; k < config.numParticles.z; k++) {
                
                Particle p;

                // Offset from base
                p.pos = base_pos + vec3(i*dx, j*dy, k*dz);
                p.pPos = base_pos + vec3(i*dx, j*dy, k*dz);

                // Uniform distribution of mass
                p.invmass = config.invmass;
                p.velocity = vec3(0.f, 0.f, 0.f);
                p.phase = config.phase;
                p.numBoundConstraints = 0;
                p.radius = min(dx, min(dy, dz)) / 2;

                addParticle(p, particles);
            }
        }
    }


    // Create constraints
    float stiffness = config.stiffness;
    float distanceThreshold = config.distanceThreshold;
    float maxDist = sqrt(dx*dx + dy*dy + dz*dz);

    std::vector<vec3> &position = particles.position;
    std::vector<tbb::atomic<int>> &numBoundConstraints = particles.numBoundConstraints;

    for (unsigned int i = start; i < particles.cardinality; i++) 
    {
        for (unsigned int j = i + 1; j < particles.cardinality; j++) 
        {
            if (glm::distance(particles.position[i], particles.position[j]) <= maxDist + 0.0001)
            {
                DistanceConstraint constraint;
                constraint.firstParticleIndex  = i;
                constraint.secondParticleIndex = j;
                constraint.stiffness = config.stiffness;
                constraint.distance  = glm::distance(position[i], position[j]);
                constraint.threshold = config.distanceThreshold;
                constraint.equality  = true;

                addConstraint(constraints.distanceConstraints, constraint);

                numBoundConstraints[i]++;
                numBoundConstraints[j]++;
            }
        }
    }

for (int i = 0; i < particles.position.size(); i++) {
    particles.position[i] *= 4;
    particles.pPosition[i] *= 4;
}
}
