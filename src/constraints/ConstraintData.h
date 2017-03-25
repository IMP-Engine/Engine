#pragma once
#include "DistanceConstraintData.h"


class ConstraintData
{
public:
    ConstraintData();
    ~ConstraintData();
    DistanceConstraintData distanceConstraints;

    void clear();
    void removeBroken(ParticleData &particleData);
};

