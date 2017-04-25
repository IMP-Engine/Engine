#include "grid.h"

Grid::Grid(int numCellsSide)
{
    this->numCellsSide = numCellsSide;
    this->numCells = numCellsSide * numCellsSide * numCellsSide;
    for (int i = 0; i < this->numCells; ++i) {
        if (i % numCellsSide == 0)
        {
            grid.push_back(std::vector<int>());
            grid.push_back(std::vector<int>());
        }
        grid.push_back(std::vector<int>());
    }
}

void Grid::buildGrid(ParticleData &particles, BoundingVolume bv)
{
    for (int i = 0; i < numCells; ++i)
    {
        grid[i].clear();
    }

    int x, y, z;
    for (int i = 0; i < particles.cardinality; ++i)
    {
        x = (particles.pPosition[i].x * numCellsSide / bv.length) - bv.corner.x;
        y = (particles.pPosition[i].y * numCellsSide / bv.length) - bv.corner.y;
        z = (particles.pPosition[i].z * numCellsSide / bv.length) - bv.corner.z;

        grid[x + numCellsSide * y + numCellsSide * numCellsSide * z].push_back(i);
    }
}

void Grid::findCollisions(DistanceConstraintData &constraints, ParticleData &particles, bool &ignorePhase)
{
    for (int i = 0; i < numCells; ++i)
    {
        if (i % numCellsSide == 0)
            continue;
        if (i % numCellsSide + 2 == 0)
            continue;

        for (int x = -1; x < 2; x++)
        {
            for (int y = -1; y < 2; y++)
            {
                for (int z = -1; z < 2; z++)
                {
                    for (int j = 0; j < grid[i].size(); ++j) {
                        int xyz = i + x + y * numCellsSide + z * numCellsSide * numCellsSide;
                        for (int k = 0; k < grid[xyz].size(); ++k)
                        {
                            Intersection isect;
                            int idx1 = grid[i][j];
                            int idx2 = grid[xyz][k];
                            if ((ignorePhase || particles.phase[idx1] != particles.phase[idx2])
                                    && ::intersect(particles, idx1, idx2, isect))
                            {
                                DistanceConstraint c;
                                c.firstParticleIndex = idx1;
                                c.secondParticleIndex = idx2;
                                c.equality = false;
                                c.distance = particledata.radius[idx1] + particledata.radius[idx2];
                                particledata.numBoundConstraints[idx1]++;
                                particledata.numBoundConstraints[idx2]++;

                                addConstraint(constraints, c);
                            }
                        }
                    }
                }
            }
        }

        /*
        [i]
        [i - 1]
        [i + 1]
        [i - numCellsSide]
        [i + numCellsSide]
        [i - numCellsSide - 1]
        [i - numCellsSide + 1]
        [i + numCellsSide - 1]
        [i + numCellsSide + 1]

        [i + numCellsSide * numCellsSide]
        [i + numCellsSide * numCellsSide - 1]
        [i + numCellsSide * numCellsSide + 1]
        [i + numCellsSide * numCellsSide - numCellsSide]
        [i + numCellsSide * numCellsSide + numCellsSide]
        [i + numCellsSide * numCellsSide - numCellsSide - 1]
        [i + numCellsSide * numCellsSide - numCellsSide + 1]
        [i + numCellsSide * numCellsSide + numCellsSide - 1]
        [i + numCellsSide * numCellsSide + numCellsSide + 1]

        [i - numCellsSide * numCellsSide]
        [i - numCellsSide * numCellsSide - 1]
        [i - numCellsSide * numCellsSide + 1]
        [i - numCellsSide * numCellsSide - numCellsSide]
        [i - numCellsSide * numCellsSide + numCellsSide]
        [i - numCellsSide * numCellsSide - numCellsSide - 1]
        [i - numCellsSide * numCellsSide - numCellsSide + 1]
        [i - numCellsSide * numCellsSide + numCellsSide - 1]
        [i - numCellsSide * numCellsSide + numCellsSide + 1]
        */
    }
}
