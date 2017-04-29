#include "KDTree.h"

#define KD_AXIS_X 0
#define KD_AXIS_Y 1
#define KD_AXIS_Z 2

// #define DEBUG_PRINT 1

void indent(int k) {
    for (int i = 0; i < k; i++)
    {
        std::cout << "    |";
    }
}

void KDTree::KDNode::print(int depth) {
    std::cout << "Node: " << particles.size() << std::endl;
    if (!leaf) {
        indent(depth);
        std::cout << "Left ";
        left->print(depth + 1);

        indent(depth);
        std::cout << "Right ";
        right->print(depth + 1);
    }
}

int KDTree::KDNode::getDepth() {
	return getDepth(0);
}

int KDTree::KDNode::getDepth(int current) {
	if (leaf) {
		return current;
	}

	return max(
		left->getDepth(current + 1), 
		right->getDepth(current + 1)
	);
}

int KDTree::KDNode::getNumInLargestLeaf() {
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

void KDTree::KDNode::subdivide(ParticleData &particleData) {
	return subdivide(0, particleData);
}

void KDTree::KDNode::subdivide(int depth, ParticleData &particleData)
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

	bool stopSubdivision = particles.size() < numParticlesInLeafBreakpoint
                        || depth            > maxTreeDepth;
	if (particles.size() == 1 || stopSubdivision) {
		
		left = new KDNode(maxTreeDepth, numParticlesInLeafBreakpoint);
		left->particles = std::vector<int>();
		
		right = new KDNode(maxTreeDepth, numParticlesInLeafBreakpoint);
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
    mid = temp[n/2].second;

    if (n % 2 == 0) {
        mid += temp[n/2 + 1].second;
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

	left = new KDNode(maxTreeDepth, numParticlesInLeafBreakpoint);
	right = new KDNode(maxTreeDepth, numParticlesInLeafBreakpoint);

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

	if (left->particles.size() > 0) {
		left->subdivide(depth + 1, particleData);
	}
	else {
		left->leaf = true;
	}


	if (right->particles.size() > 0) {
		right->subdivide(depth + 1, particleData);
	}
	else {
		right->leaf = true;
	}
}


void KDTree::KDNode::findCollisionsForParticle(int firstIndex, ParticleData &particleData, DistanceConstraintData &constraints, bool ignorePhase, int depth) {
	bool hit = false;
    if (!leaf)
    {
        float particleCoord = particleData.position[firstIndex][depth % 3];
        if (particleCoord < mid || mid - particleCoord <= particleData.radius[firstIndex]) {
#ifdef DEBUG_PRINT
            std::cout << "Checking left at depth " << depth << std::endl;
#endif // DEBUG_PRINT
		    left->findCollisionsForParticle(firstIndex,  particleData, constraints, ignorePhase, depth + 1);

        }
        if (particleCoord >= mid || particleCoord - mid <= particleData.radius[firstIndex]) {
#ifdef DEBUG_PRINT
            std::cout << "Checking right at depth " << depth << std::endl;
#endif // DEBUG_PRINT
            right->findCollisionsForParticle(firstIndex, particleData, constraints, ignorePhase, depth + 1);
        }
    }
    else
    {
        for (auto secondIndex : particles)
        {
            Intersection isect;

            if ((ignorePhase || particleData.phase[firstIndex] != particleData.phase[secondIndex])
                && ::intersect(particleData, firstIndex, secondIndex, isect))
            {
                DistanceConstraint c;
                c.firstParticleIndex = firstIndex;
                c.secondParticleIndex = secondIndex;
                c.equality = false;
                c.distance = particleData.radius[firstIndex] + particleData.radius[secondIndex];
                particleData.numBoundConstraints[firstIndex]++;
                particleData.numBoundConstraints[secondIndex]++;

                addConstraint(constraints, c);
            }
        }
    }
}

KDTree::KDNode *KDTree::build(ParticleData &particles) {
#ifdef DEBUG_PRINT
    std::cout << "Building k-d tree";
#endif // DEBUG_PRINT


	root = new KDNode(maxTreeDepth, numParticlesInLeafBreakpoint);
	root->particles = std::vector<int>();

	for (unsigned int i = 1; i < particles.cardinality; i++)
	{
		root->particles.push_back(i);
	}


	root->subdivide(particles);
#ifdef DEBUG_PRINT
	std::cout << "Tree max depth: " << root->getDepth() << "\n";
	std::cout << "Biggest leaf: " << root->getNumInLargestLeaf() << "\n";
    root->print(0);
#endif
    return root;
}


void KDTree::findCollisions(ParticleData &particles, DistanceConstraintData &constraints, bool ignorePhase)
{
    for (unsigned int i = 0; i < particles.cardinality; i++)
    {
#ifdef DEBUG_PRINT
        std::cout << "Finding collisions for patricle " << i << std::endl;
        int before = constraints.cardinality;
#endif // DEBUG_PRINT

	    root->findCollisionsForParticle(i, particles, constraints, ignorePhase, 0);

#ifdef DEBUG_PRINT
        std::cout << "Collisions found: " << constraints.cardinality - before << std::endl;
#endif // DEBUG_PRINT
    }
}

