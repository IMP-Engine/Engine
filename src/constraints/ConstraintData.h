#pragma once
#include "DistanceConstraintData.h"
#include "PlaneCollisionConstraintData.h"
#include "FixedPointConstraintData.h"

class ConstraintData
{
public:
    ConstraintData();
    ~ConstraintData();
    DistanceConstraintData distanceConstraints;
    DistanceConstraintData particleCollisionConstraints;
	PlaneCollisionConstraintData planeCollisionConstraints;
    FixedPointConstraintData fixedPointConstraints;

    void clear();
    void removeBroken(ParticleData &particleData);
};

