#pragma once

#include "../particles/ParticleData.h"
#include "../constraints/DistanceConstraintData.h"
#include "../intersections.h"


void bruteForce(ParticleData & particles, DistanceConstraintData & constraints, bool ignorePhase);
