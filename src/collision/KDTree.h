#pragma once
#include <algorithm>
#include "../particles/ParticleData.h"
#include "../constraints/DistanceConstraintData.h"
#include "../primitives/BoundingVolume.h"
#include "glm/glm.hpp"



class KDNode {
public:
	KDNode();
	~KDNode();

    static int KDNode::MAX_TREE_DEPTH;
    static int KDNode::NUM_TRIANGLE_LEAF_BREAKPOINT;


	bool leaf;
	KDNode *left;
	KDNode *right;
	std::vector<int> particles;
    glm::vec3 boxMin, boxMax;


	void subdivide(ParticleData &particleData);
	void subdivide(int depth, ParticleData &particleData);
	bool intersect(int particleIndex, ParticleData &particleData, DistanceConstraintData &constraints);

	int KDNode::getDepth();
	int KDNode::getDepth(int current);

	int KDNode::getNumInLargestLeaf();

};

class KDTree
{
public:
	 KDTree() {};
	~KDTree() {};

	bool build(ParticleData &particles);
	bool intersect(int particleIndex, ParticleData particleData, DistanceConstraintData constraints);

    int maxTreeDepth;
    int numParticlesInLeafBreakpoint;

private:
	KDNode *root;
};

