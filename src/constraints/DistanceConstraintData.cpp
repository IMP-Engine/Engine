#include "DistanceConstraintData.h"



DistanceConstraintData::DistanceConstraintData()
{
}


DistanceConstraintData::~DistanceConstraintData()
{
}

glm::vec3 DistanceConstraintData::gradient(int constraintIndex, int particleIndex)
{
    glm::vec3 p1 = this->particles[0]->pPos;
    glm::vec3 p2 = this->particles[1]->pPos;

    glm::vec3 c = (p1 - p2) / length(p1 - p2);

    return (p == this->particles.begin() ? c : -c);
    return glm::vec3();
}

glm::vec3 DistanceConstraintData::scaleFactor(int constraintIndex)
{
    return glm::vec3();
}
