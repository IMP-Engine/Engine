#pragma once

#include <vector>
#include "glm/glm.hpp"

#include "../particles/ParticleData.h"

#ifndef MAX_DISTANCE_CONSTRAINTS
#define MAX_DISTANCE_CONSTRAINTS 200000
#endif // !MAX_DISTANCE_CONSTRAINTS


class DistanceConstraintData
{
public:
    DistanceConstraintData();
    ~DistanceConstraintData();

    int cardinality;

    std::vector<ivec2> particles; // Assume there is 2 particles 
    std::vector<float> elasticity;
    std::vector<float> distance;
    std::vector<bool>  equality;
    std::vector<float> threshold;

    float evaluate(int constraintIndex, ParticleData &particleData);
    glm::vec3 gradient(int constraintIndex, int particleIndex, ParticleData &particleData);
    float scaleFactor(int constraintIndex, ParticleData &particleData, float lambda, double dt);

    void clear();
    void removeBroken(ParticleData &particleData);

};
struct DistanceConstraint {
    int firstParticleIndex;
    int secondParticleIndex;
    float elasticity;
    float distance;
    float threshold;
    bool equality;
};


void addConstraint(DistanceConstraintData &data, DistanceConstraint &config);

