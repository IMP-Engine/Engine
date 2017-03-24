#pragma once

#include <vector>
#include "glm/glm.hpp"

class DistanceConstraintData
{
public:
    DistanceConstraintData();
    ~DistanceConstraintData();


    std::vector<int[3]> particles;
    std::vector<float> stiffness;
    std::vector<float> distance;
    std::vector<bool> equality;
    std::vector<float> threshold;

    glm::vec3 gradient(int constraintIndex, int particleIndex);
    glm::vec3 scaleFactor(int constraintIndex);


};

