#pragma once

#include <vector>
#include "glm/glm.hpp"

#include "../particles/ParticleData.h"

#ifndef MAX_DISTANCE_CONSTRAINTS
#define MAX_DISTANCE_CONSTRAINTS 5000
#endif // !MAX_DISTANCE_CONSTRAINTS


class PlaneCollisionConstraintData
{
public:
	PlaneCollisionConstraintData();

	int cardinality;

	std::vector<unsigned int> particles;
	std::vector<glm::vec3> normals;
	std::vector<float> distances;

	bool solvePlaneCollision(glm::vec3 &delta, const int idx, ParticleData &particleData);

	void clear();

};
struct PlaneCollisionConstraint {
	int particleIndex;
	vec3 normal;
	float distance;
};

void addConstraint(PlaneCollisionConstraintData &data, PlaneCollisionConstraint &config);

