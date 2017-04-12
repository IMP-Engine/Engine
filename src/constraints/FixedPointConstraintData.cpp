#include "FixedPointConstraintData.h"



FixedPointConstraintData::FixedPointConstraintData()
{
    particle.reserve(MAX_FIXED_POINT_CONSTRAINTS);
    position.reserve(MAX_FIXED_POINT_CONSTRAINTS);
    cardinality = 0;
}
FixedPointConstraintData::~FixedPointConstraintData() { }



float FixedPointConstraintData::evaluate(int constraintIndex, ParticleData &particleData)
{
    int firstParticleIndex = particle[constraintIndex];

    glm::vec3 p1 = particleData.pPosition[firstParticleIndex];
    glm::vec3 p2 = position[constraintIndex];

    return length(p1 - p2);
}


bool FixedPointConstraintData::solveDistanceConstraint(int constraintIndex, glm::vec3 & delta, ParticleData & particleData)
{
    int firstParticleIndex = particle[constraintIndex];

    glm::vec3 p1 = particleData.pPosition[firstParticleIndex];
    glm::vec3 p2 = position[constraintIndex];

    vec3 diff = p1 - p2;

    float c = length(diff);
    
    vec3 grad = glm::normalize(diff);

    delta = c * grad;

    return true;
}


void FixedPointConstraintData::clear()
{
    particle.clear();
    position.clear();
    cardinality = 0;
}



void addConstraint(FixedPointConstraintData &data, FixedPointConstraint &config)
{
    data.particle.push_back(config.particle);
    data.position.push_back(config.position);
    data.cardinality++;
}
