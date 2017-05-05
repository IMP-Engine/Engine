#pragma once

#include <vector>
#include "../../tbb/include/tbb/parallel_for.h"
#include "../tbb/include/tbb/mutex.h"
#include "../tbb/include/tbb/concurrent_vector.h"
#include "../particles/ParticleData.h"
#include "../constraints/DistanceConstraintData.h"
#include "../intersections.h"
#include "../primitives/BoundingVolume.h"

class Grid
{
    public:
        std::vector<tbb::concurrent_vector<int>> grid;
        int numCells;
        int numCellsSide;

        Grid(int numCellsSide);
        void buildGrid(ParticleData &particles, BoundingVolume bv, bool parallel);
        void findCollisions(DistanceConstraintData &constraints, ParticleData &particles, bool &ignorePhase, bool parallel);
};
