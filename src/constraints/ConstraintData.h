#pragma once
#include "DistanceConstraintData.h"
#include "PlaneCollisionConstraintData.h"

class ConstraintData
{
public:
    ConstraintData();
    ~ConstraintData();
    DistanceConstraintData distanceConstraints;
	PlaneCollisionConstraintData triangleCollisionConstraints;

    void clear();
    void removeBroken(ParticleData &particleData);
};

