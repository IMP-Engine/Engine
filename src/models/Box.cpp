#include "Box.h"


void Box::makeBox(ParticleData &particles, ConstraintData &constraints, model::modelConfig config) {
 
	unsigned int start = particles.cardinality;

    float dx = 0, dy = 0, dz = 0;

    if (config.numParticles.x > 1) {
        dx = config.scale.x / (config.numParticles.x-1);
    }
    if (config.numParticles.y > 1) {
        dy = config.scale.y / (config.numParticles.y-1);
    }
    if (config.numParticles.z > 1) {
        dz = config.scale.z / (config.numParticles.z-1);
    }

    // Get the "minimum" corner to use as base position
    vec3 base_pos = config.centerPos - (config.scale * 0.5f);

    // Create particles
    for (int i = 0; i < config.numParticles.x; i++) {
        for (int j = 0; j < config.numParticles.y; j++) {
            for (int k = 0; k < config.numParticles.z; k++) {
                
				Particle p;

                // Offset from base
                p.pos = base_pos + vec3(i*dx, j*dy, k*dz);

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
    float elasticity = config.elasticity;
    float distanceThreshold = config.distanceThreshold;
	float maxDist = sqrt(dx*dx + dy*dy + dz*dz);

    std::vector<vec3> &position = particles.position;
    std::vector<int> &numBoundConstraints = particles.numBoundConstraints;

    for (unsigned int i = start; i < particles.cardinality; i++) 
    {
        for (unsigned int j = i + 1; j < particles.cardinality; j++) 
        {
            if (glm::distance(particles.position[i], particles.position[j]) <= maxDist)
            {
                DistanceConstraint constraint;
                constraint.firstParticleIndex  = i;
                constraint.secondParticleIndex = j;
                constraint.elasticity = config.elasticity;
                constraint.distance  = glm::distance(position[i], position[j]);
                constraint.threshold = config.distanceThreshold;
                constraint.equality  = true;

                addConstraint(constraints.distanceConstraints, constraint);

                numBoundConstraints[i]++;
                numBoundConstraints[j]++;
            }
        }
    }


    /* 
        The following code is a bit of a hack to att corner to corner constraints
        These help alot in preventing "folding" of the cube.
    */



    ivec3 n = config.numParticles;
 

    int pairs[] = {
              0 * n.y * n.z +   0 * n.z +       0, (n.x-1) * n.y * n.z + (n.y-1) * n.z + (n.z-1),
        (n.x-1) * n.y * n.z +   0 * n.z +       0,       0 * n.y * n.z + (n.y-1) * n.z + (n.z-1),
        (n.x-1) * n.y * n.z +   0 * n.z + (n.z-1),       0 * n.y * n.z + (n.y-1) * n.z +       0,
              0 * n.y * n.z +   0 * n.z + (n.z-1), (n.x-1) * n.y * n.z + (n.y-1) * n.z +       0,
    };

    for (int k = 0; k < 8; k+=2)
    {
        int i = pairs[k];
        int j = pairs[k + 1];

        DistanceConstraint constraint;
        constraint.firstParticleIndex  = i;
        constraint.secondParticleIndex = j;
        constraint.elasticity = config.elasticity;
        constraint.distance  = glm::distance(position[i], position[j]);
        constraint.threshold = config.distanceThreshold;
        constraint.equality  = true;

        addConstraint(constraints.distanceConstraints, constraint);

        numBoundConstraints[i]++;
        numBoundConstraints[j]++;
    }
}