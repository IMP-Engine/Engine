#pragma once
#include "PlaneCollisionConstraintData.h"
#include "../imgui/imgui.h"

PlaneCollisionConstraintData::PlaneCollisionConstraintData()
{
	particles.reserve(MAX_DISTANCE_CONSTRAINTS);
	normals.reserve(MAX_DISTANCE_CONSTRAINTS);
	distances.reserve(MAX_DISTANCE_CONSTRAINTS);
	cardinality = 0;
}

bool PlaneCollisionConstraintData::solvePlaneCollision(glm::vec3 & delta, const int idx, ParticleData & particleData)
{
	float c = glm::dot(normals[idx], particleData.pPosition[particles[idx]]) - distances[idx];

	if (c > 0)
		return false;

	delta = c * normals[idx];
	return true;
}

void PlaneCollisionConstraintData::clear()
{
	particles.clear();
	normals.clear();
	distances.clear();
	cardinality = 0;
}

void addConstraint(PlaneCollisionConstraintData & data, PlaneCollisionConstraint & config)
{
	data.particles.push_back(config.particleIndex);
	data.normals.push_back(config.normal);
	data.distances.push_back(config.distance);
	data.cardinality++;
}
