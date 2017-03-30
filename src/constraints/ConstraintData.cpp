#include "ConstraintData.h"



ConstraintData::ConstraintData()
{
}


ConstraintData::~ConstraintData()
{
}

void ConstraintData::clear()
{
    distanceConstraints.clear();
}

void ConstraintData::removeBroken(ParticleData &particleData)
{
    distanceConstraints.removeBroken(particleData);
}
