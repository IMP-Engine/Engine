#pragma once

#include <vector>
#include "glm/glm.hpp"

#include "../../tbb/include/tbb/mutex.h"
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
    std::vector<float> stiffness;
    std::vector<float> distance;
    std::vector<bool>  equality;
    std::vector<float> threshold;

    float evaluate(int constraintIndex, ParticleData &particleData);

    bool solveDistanceConstraint(glm::vec3 &delta1, glm::vec3 &delta2, int constraintIndex, ParticleData &particleData);

    void clear();
    void removeBroken(ParticleData &particleData);

};
struct DistanceConstraint {
    int firstParticleIndex;
    int secondParticleIndex;
    float stiffness;
    float distance;
    float threshold;
    bool equality;
};


void addConstraint(DistanceConstraintData &data, DistanceConstraint &config);

