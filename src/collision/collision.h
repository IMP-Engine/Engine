#pragma once

#include <string>

#include "../imgui/imgui.h"

#include "../particles/ParticleData.h"
#include "../constraints/DistanceConstraintData.h"

#include "../intersections.h"

#include "bruteforce.h"
#include "octree.h"

namespace collision
{
    enum collisionDetectionType {BruteForce, Octree};

    void gui(bool* show);
    void createCollisionConstraints(ParticleData & particles, DistanceConstraintData & constraints);
};