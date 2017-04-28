#include "KDTree.h"

#define KD_AXIS_X 0
#define KD_AXIS_Y 1
#define KD_AXIS_Z 2

#define DEBUG_PRINT_ 1

int KDNode::MAX_TREE_DEPTH = 50;
int KDNode::NUM_TRIANGLE_LEAF_BREAKPOINT = 3;


KDNode::KDNode() {
}

KDNode::~KDNode() {
}

int KDNode::getDepth() {
	return getDepth(0);
}

int KDNode::getDepth(int current) {
	if (leaf) {
		return current;
	}

	return max(
		left->getDepth(current + 1), 
		right->getDepth(current + 1)
	);
}

int KDNode::getNumInLargestLeaf() {
	if (leaf) {
		return particles.size();
	}
	else {
		return max(
            left->getNumInLargestLeaf(), 
            right->getNumInLargestLeaf()
        );
	}
}

void KDNode::subdivide(ParticleData &particleData) {
	return subdivide(0, particleData);
}

void KDNode::subdivide(int depth, ParticleData &particleData)
{
	//cout << "Subdividing " << triangles.size() << " triangles at depth " << depth << "\n";
    int axis = depth % 3;
	left = NULL;
	right = NULL;
	leaf = true;

	// Empty node
	if (particles.size() == 0) {
		return;
	}

	bool stopSubdivision = particles.size() < NUM_TRIANGLE_LEAF_BREAKPOINT 
                        || depth            > MAX_TREE_DEPTH;
	if (particles.size() == 1 || stopSubdivision) {
		
		left = new KDNode();
		left->particles = std::vector<int>();
		
		right = new KDNode();
		right->particles = std::vector<int>();

		return;
	}
	
    typedef std::pair<int, float> indexPosition;
    std::vector<indexPosition> temp;

    for (unsigned int i = 0; i < particles.size(); i++) {
        temp.push_back(
            indexPosition(
                particles[i],
                particleData.position[particles[i]][axis]
                )
        );
    }
    std::sort(
        temp.begin(), 
        temp.end(), 
        [axis] (indexPosition a, indexPosition b) {
            return a.second < b.second;
        }
    );


	// No possible subdivision, return a leaf.

	std::vector<int> leftParticles, rightParticles;


    unsigned int n = temp.size();
    float mid = temp[n].second;

    if (n % 2 == 0) {
        mid += temp[n + 1].second;
        mid /= 2;
    }

	// Split the triangles based on center point
	for (auto it : particles)
	{

		if (particleData.position[it][axis] <= mid) {
			leftParticles.push_back(it);
		} else {
			rightParticles.push_back(it);
		}

	}

	left = new KDNode();
	right = new KDNode();

    left->boxMin = boxMin;
    left->boxMax = boxMax;
    left->boxMax[axis] = mid;

	left->particles = std::vector<int>(leftParticles);

    right->boxMin = boxMin;
    right->boxMax = boxMax;
    right->boxMin[axis] = mid;
	right->particles = std::vector<int>(rightParticles);

	leaf = false;

	bool equalFactor = false;

	if (leftParticles.size() > 0) {
		left->subdivide(depth + 1, particleData);
	}
	else {
		left->leaf = true;
	}


	if (rightParticles.size() > 0) { 
		right->subdivide(depth + 1, particleData);
	}
	else {
		right->leaf = true;
	}
}


bool KDNode::intersect(int particleIndex, ParticleData &particleData, DistanceConstraintData &constraints) {
	bool hit = false;
	/*
    if (!aabb.intersect(r)) {
		return false;
	}
	if (!leaf) {

		bool hitLeft = left->aabb.intersect(r);
		bool hitRight = right->aabb.intersect(r);
		bool hit = false;

		if (hitLeft) {
			hit |= left->intersect(r, isect);
		}
		if (hitRight) {
			hit |= right->intersect(r, isect);
		}
		return hit;
	}

	for (auto &it : triangles)
	{
		hit |= it->intersect(r, isect);
	}
    */
	return hit;
}

bool KDTree::build(ParticleData &particles) {
#ifdef DEBUG_PRINT
    std::cout << "Building k-d tree";
#endif // DEBUG_PRINT

	KDNode::MAX_TREE_DEPTH = maxTreeDepth;
	KDNode::NUM_TRIANGLE_LEAF_BREAKPOINT = numParticlesInLeafBreakpoint;

	root = new KDNode(); 
	root->particles = std::vector<int>();

	for (unsigned int i = 1; i < particles.cardinality; i++)
	{
		root->particles.push_back(i);
	}


	root->subdivide(particles);
#ifdef DEBUG_PRINT
	cout << "Tree max depth: " << root->getDepth() << "\n";
	cout << "Biggest leaf: " << root->getNumInLargestLeaf() << "\n";
#endif

	return true;
}

bool KDTree::intersect(int particleIndex, ParticleData particleData, DistanceConstraintData constraints)
{
	return root->intersect(particleIndex, particleData, constraints);
}

