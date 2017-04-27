#pragma once
#include <vector>
#include <algorithm>
#include <iterator>

#include <glm/common.hpp>

#include "../primitives/BoundingVolume.h"
#include "../particles/ParticleData.h"
#include "../constraints/DistanceConstraintData.h"
#include "../intersections.h"

class Octree 
{
public:
    class Node
    {
    public:
        BoundingVolume bv;
        Node *children[8];
        std::vector<int> particles;

        Node(BoundingVolume bv) { this->bv = bv; for (int i = 0; i < 8; i++) children[i] = nullptr; }

        void construct(std::vector<glm::vec3> & positions, std::vector<float> & radii, std::vector<int> containedIndices);
        void findCollisions(ParticleData & particledata, DistanceConstraintData & constraints);

        ~Node() {
   
            for (int i = 0; i < 8; i++) {
                delete children[i];
            }

        }
    };

    Node *root;
    
    Node* getRoot() { return root; }
    void construct(ParticleData & particles, BoundingVolume bv, int numParticles, float minVolume, bool ignorePhase);
    void findCollisions(ParticleData & particledata, DistanceConstraintData & constraints);
	
    ~Octree() { delete root; }
};