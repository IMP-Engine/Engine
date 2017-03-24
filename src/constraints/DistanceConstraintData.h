#pragma once

#include <vector>
#include "glm/glm.hpp"

#include "../particles/ParticleData.h"

class DistanceConstraintData
{
public:
    DistanceConstraintData();
    ~DistanceConstraintData();

    int cardinality;

    std::vector<std::vector<int>> particles; // Assume there is 2 particles 
    std::vector<float>  stiffness;
    std::vector<float>  distance;
    std::vector<bool>   equality;
    std::vector<float>  threshold;

    glm::vec3 gradient(int constraintIndex, int particleIndex, ParticleData &particleData);
    float scaleFactor(int constraintIndex, ParticleData &particleData);

    void clear();

};

void addConstraint(DistanceConstraintData data, int firstParticleIndex, int secondParticleIndex, float stiffness, float distance, float threshold, bool equality);

