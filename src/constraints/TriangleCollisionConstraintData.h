#pragma once

#include <vector>
#include "glm/glm.hpp"

#include "../particles/ParticleData.h"

#ifndef MAX_DISTANCE_CONSTRAINTS
#define MAX_DISTANCE_CONSTRAINTS 5000
#endif // !MAX_DISTANCE_CONSTRAINTS


class TriangleCollisionConstraintData
{
public:
	TriangleCollisionConstraintData();

	int cardinality;

	std::vector<unsigned int> particles;
	std::vector<glm::vec3> normals;
	std::vector<float> distances;

	bool solveTriangleCollision(glm::vec3 &delta, const int idx, ParticleData &particleData, float a);

	void clear();

};
struct TriangleCollisionConstraint {
	int particleIndex;
	vec3 normal;
	float distance;
};

void addConstraint(TriangleCollisionConstraintData &data, TriangleCollisionConstraint &config);

