#pragma once
#include "DistanceConstraintData.h"
#include "PlaneCollisionConstraintData.h"

class ConstraintData
{
public:
    ConstraintData();
    ~ConstraintData();
    DistanceConstraintData distanceConstraints;
    DistanceConstraintData particleCollisionConstraints;
	PlaneCollisionConstraintData planeCollisionConstraints;

    void clear();
    void removeBroken(ParticleData &particleData);
};

