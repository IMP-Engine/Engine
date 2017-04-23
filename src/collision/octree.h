#pragma once
#include <vector>
#include <algorithm>
#include <iterator>

#include <glm/common.hpp>

#include "../primitives/BoundingVolume.h"
#include "../particles/ParticleData.h"
#include "../constraints/ConstraintData.h"
#include "../constraints/DistanceConstraintData.h"

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
        ~Node() {
   
            for (int i = 0; i < 8; i++) {
                delete children[i];
            }

        }
    };

    Node *root;
    
    Node* getRoot() { return root; }
    /* Construct tree, add collisions to constraints*/
    void collide(ParticleData & particles, ConstraintData & constraints, BoundingVolume bv) { construct(particles, bv, 0, 0); findCollisions(constraints); };
    void construct(ParticleData & particles, BoundingVolume bv, int numParticles, float minVolume);
    void findCollisions(ConstraintData & constraints);
	
    ~Octree() { delete root; }
};