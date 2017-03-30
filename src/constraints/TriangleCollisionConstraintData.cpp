#pragma once
#include "TriangleCollisionConstraintData.h"
#include "../imgui/imgui.h"

TriangleCollisionConstraintData::TriangleCollisionConstraintData()
{
	particles.reserve(MAX_DISTANCE_CONSTRAINTS);
	normals.reserve(MAX_DISTANCE_CONSTRAINTS);
	distances.reserve(MAX_DISTANCE_CONSTRAINTS);
	cardinality = 0;
}

bool TriangleCollisionConstraintData::solveTriangleCollision(glm::vec3 & delta, const int idx, ParticleData & particleData, float a)
{
	float c = glm::dot(normals[idx], particleData.pPosition[particles[idx]]) - distances[idx];

	if (c > 0)
		return false;
	


	delta = c * normals[idx] * a;
	return true;
	
}

void TriangleCollisionConstraintData::clear()
{
	particles.clear();
	normals.clear();
	distances.clear();
	cardinality = 0;
}

void addConstraint(TriangleCollisionConstraintData & data, TriangleCollisionConstraint & config)
{
	data.particles.push_back(config.particleIndex);
	data.normals.push_back(config.normal);
	data.distances.push_back(config.distance);
	data.cardinality++;
}
