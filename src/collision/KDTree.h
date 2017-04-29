#pragma once
#include <algorithm>
#include "../particles/ParticleData.h"
#include "../constraints/DistanceConstraintData.h"
#include "../intersections/SphereIntersection.h"
#include "../primitives/BoundingVolume.h"
#include <iostream>
#include "glm/glm.hpp"


class KDTree
{
public:

    class KDNode {
    public:
        KDNode(int maxDepth, unsigned int breakpoint) 
            : maxTreeDepth(maxDepth),
              numParticlesInLeafBreakpoint(breakpoint)
        {};
        ~KDNode() {};

        int maxTreeDepth;
        unsigned int numParticlesInLeafBreakpoint;


	    bool leaf;
	    KDNode *left;
	    KDNode *right;
        float mid;

	    std::vector<int> particles;
        glm::vec3 boxMin, boxMax;


	    void subdivide(ParticleData &particleData);
	    void subdivide(int depth, ParticleData &particleData);
	    void findCollisionsForParticle(int particleIndex, ParticleData &particleData, DistanceConstraintData &constraints, bool ignorePhase, int depth);
        void print(int depth);

	    int KDNode::getDepth();
	    int KDNode::getDepth(int current);

	    int KDNode::getNumInLargestLeaf();

    };

	 KDTree() {};
	~KDTree() {};

    KDNode *build(ParticleData &particles);
    void findCollisions(ParticleData &particles, DistanceConstraintData &constraints, bool ignorePhase);

    int maxTreeDepth;
    int unsigned numParticlesInLeafBreakpoint;

	KDNode *root;
};

