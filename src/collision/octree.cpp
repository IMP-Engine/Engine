#include "Octree.h"

int numParticles= 1;
float minVolume = 0.1;

void Octree::construct(ParticleData & particles, BoundingVolume bv, int a, float b)
{
    numParticles = a;
    minVolume = b;
    this->root = new Node(bv);
    std::vector<int> containedIndices(particles.cardinality);
    for (int i = 0; i < particles.cardinality; i++) {
        containedIndices[i] = i;
    }

    this->root->construct(particles.pPosition, particles.radius, containedIndices);
}

void Octree::Node::construct(std::vector<glm::vec3> & positions, std::vector<float> & radii, std::vector<int> containedIndices)
{
    // When to stop
    if (containedIndices.size() < numParticles)
        return;
    // How many particles are lost here? TODO set contained indices to our list?
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
