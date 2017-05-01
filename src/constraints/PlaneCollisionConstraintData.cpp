#pragma once
#include "PlaneCollisionConstraintData.h"

PlaneCollisionConstraintData::PlaneCollisionConstraintData()
{
    particles.reserve(MAX_DISTANCE_CONSTRAINTS);
    normals.reserve(MAX_DISTANCE_CONSTRAINTS);
    distances.reserve(MAX_DISTANCE_CONSTRAINTS);
    cardinality = 0;
}

bool PlaneCollisionConstraintData::solvePlaneCollision(glm::vec3 & delta, const int idx, ParticleData & particleData, bool stabilize)
{
    float c = glm::dot(normals[idx], particleData.pPosition[particles[idx]]) - distances[idx];

    if (c > 0)
        return false;

    if (stabilize) // Calculate delta with position instead of pPosition
        c = glm::dot(normals[idx], particleData.position[particles[idx]]) - distances[idx];

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

void PlaneCollisionConstraintData::addConstraint(PlaneCollisionConstraint & config)
{
    particles.push_back(config.particleIndex);
    normals.push_back(config.normal);
    distances.push_back(config.distance);
    cardinality++;
}
