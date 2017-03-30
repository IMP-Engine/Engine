#pragma once
#include "DistanceConstraintData.h"
#include "TriangleCollisionConstraintData.h"

class ConstraintData
{
public:
    ConstraintData();
    ~ConstraintData();
    DistanceConstraintData distanceConstraints;
	TriangleCollisionConstraintData triangleCollisionConstraints;

    void clear();
    void removeBroken(ParticleData &particleData);
};

