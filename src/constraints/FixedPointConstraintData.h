#pragma once
#include <vector>
#include "glm/glm.hpp"
#include "../particles/ParticleData.h"

#define MAX_FIXED_POINT_CONSTRAINTS 200000

class FixedPointConstraintData
{
public:

    std::vector<int> particle;
    std::vector<glm::vec3> position;
    unsigned int cardinality;

    FixedPointConstraintData();
    ~FixedPointConstraintData();

    float evaluate(int constraintIndex, ParticleData &particleData);
    bool solveDistanceConstraint(int constraintIndex, glm::vec3 & delta1, ParticleData &particleData);
    void clear();
};

struct FixedPointConstraint {
    int particle;
    glm::vec3 position;
};
