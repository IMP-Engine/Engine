#pragma once

#include <vector>
#include "../particles/ParticleData.h"
#include "../constraints/DistanceConstraintData.h"
#include "../intersections.h"
#include "../primitives/BoundingVolume.h"

class Grid
{
    public:
        std::vector<std::vector<int>> grid;
        int numCells;
        int numCellsSide;

        Grid(int numCellsSide);
        void buildGrid(ParticleData &particles, BoundingVolume bv);
        void findCollisions(DistanceConstraintData &constraints, ParticleData &particles, bool &ignorePhase);
};
