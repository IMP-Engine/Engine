#include "octree.h"

unsigned int numParticles= 1;
float minVolume = 0.1;
bool igPhase = true;

void Octree::construct(ParticleData & particles, BoundingVolume bv, int a, float b, bool ignorePhase)
{
    numParticles = a;
    minVolume = b;
    igPhase = ignorePhase;
    this->root = new Node(bv);
    std::vector<int> containedIndices(particles.cardinality);
    for (unsigned int i = 0; i < particles.cardinality; i++) {
        containedIndices[i] = i;
    }

    this->root->construct(particles.pPosition, particles.radius, containedIndices);
}

void Octree::findCollisions(ParticleData & particledata, DistanceConstraintData & constraints)
{
    this->getRoot()->findCollisions(particledata, constraints);
}

void Octree::Node::construct(std::vector<glm::vec3> & positions, std::vector<float> & radii, std::vector<int> containedIndices)
{
    if (containedIndices.size() < numParticles) {
        this->particles.swap(containedIndices);
        return;
    }
        
    if (bv.getVolume() < minVolume) {
        this->particles.swap(containedIndices);
        return;
    }

    BoundingVolume bvs[8];
    /* Calculate children bounding volumes */
    bvs[0] = BoundingVolume(bv.corner + glm::vec3(0, 0, 0), bv.length / 2);                                      //
    bvs[1] = BoundingVolume(bv.corner + glm::vec3(bv.length/2, 0, 0), bv.length / 2);                            // x
    bvs[2] = BoundingVolume(bv.corner + glm::vec3(0, bv.length / 2, 0), bv.length / 2);                          // y
    bvs[3] = BoundingVolume(bv.corner + glm::vec3(bv.length / 2, bv.length / 2, 0), bv.length / 2);              // xy
    bvs[4] = BoundingVolume(bv.corner + glm::vec3(0, 0, bv.length / 2), bv.length / 2);                          // z
    bvs[5] = BoundingVolume(bv.corner + glm::vec3(bv.length / 2, 0, bv.length / 2), bv.length / 2);              // xz
    bvs[6] = BoundingVolume(bv.corner + glm::vec3(0, bv.length / 2, bv.length / 2), bv.length / 2);              // yz
    bvs[7] = BoundingVolume(bv.corner + glm::vec3(bv.length / 2, bv.length / 2, bv.length / 2), bv.length / 2);  // xyz
    
    std::vector< std::vector<int> > containedIndicesList;
    
    for (int i = 0; i < 8; i++) {
        children[i] = new Node(bvs[i]);
        containedIndicesList.push_back(std::vector<int>());
        // TODO eight?
        containedIndicesList[i].reserve(containedIndices.size() / 8);
    }

    for (int i : containedIndices) {
        for (int j = 0; j < 8; j++) {
            // TODO do we need to check all eight subdivisions?
            if (children[j]->bv.isSphereInside(positions[i], radii[i])) {
                containedIndicesList[j].push_back(i);
            }
        }
    }

    // Recurse
    for (int i = 0; i < 8; i++) {
        children[i]->construct(positions, radii, containedIndicesList[i]);
    }
}

void Octree::Node::findCollisions(ParticleData & particledata, DistanceConstraintData & constraints)
{
    if (children[0] == nullptr)
    {
        for (unsigned int i = 0; i < this->particles.size(); i++)
        {
            for (unsigned int j = i+1; j < this->particles.size(); j++)
            {
                Intersection isect;
                int idx1 = this->particles[i];
                int idx2 = this->particles[j];
                if ((igPhase || particledata.phase[idx1] != particledata.phase[idx2])
                    && ::intersect(particledata, idx1, idx2, isect))
                {
                    DistanceConstraint c;
                    c.firstParticleIndex = idx1;
                    c.secondParticleIndex = idx2;
                    c.equality = false;
                    c.distance = particledata.radius[idx1] + particledata.radius[idx2];
                    particledata.numBoundConstraints[idx1]++;
                    particledata.numBoundConstraints[idx2]++;

                    addConstraint(constraints, c);
                }
            }
        }
    } 
    else
    {
        for (int i = 0; i < 8; i++) 
        {
            children[i]->findCollisions(particledata, constraints);
        }
    }
}
