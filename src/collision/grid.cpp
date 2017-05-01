#include "grid.h"

tbb::mutex mutexGrid;

Grid::Grid(int numCellsSide)
{
    this->numCellsSide = numCellsSide;
    this->numCells = numCellsSide * numCellsSide * numCellsSide;
    int paddedSide = numCellsSide + 2;
    for (int i = 0; i < paddedSide * paddedSide * paddedSide; ++i) {
        grid.push_back(tbb::concurrent_vector<int>());
    }
}

void Grid::buildGrid(ParticleData &particles, BoundingVolume bv, bool parallel)
{
    for (int i = 0; i < grid.size(); ++i)
    {
        grid[i].clear();
    }

    if (parallel)
    {
        tbb::parallel_for(tbb::blocked_range<size_t>(0, particles.cardinality),
                [&](const tbb::blocked_range<size_t>& r) {
            for (size_t i = r.begin(); i != r.end(); ++i)
            {
                int x, y, z;
                x = numCellsSide / bv.length * (particles.pPosition[i].x - bv.corner.x);
                y = numCellsSide / bv.length * (particles.pPosition[i].y - bv.corner.y);
                z = numCellsSide / bv.length * (particles.pPosition[i].z - bv.corner.z);
                if (x < 0)
                    continue;
                if (y < 0)
                    continue;
                if (z < 0)
                    continue;
                if (x > numCellsSide)
                    continue;
                if (y > numCellsSide)
                    continue;
                if (z > numCellsSide)
                    continue;
                ++x;
                ++y;
                ++z;
                
                grid[x + numCellsSide * y + numCellsSide * numCellsSide * z].push_back(i);
            }
        });
    }
    else
    {
        for (int i = 0; i < particles.cardinality; ++i)
        {
            int x, y, z;
            x = numCellsSide / bv.length * (particles.pPosition[i].x - bv.corner.x);
            y = numCellsSide / bv.length * (particles.pPosition[i].y - bv.corner.y);
            z = numCellsSide / bv.length * (particles.pPosition[i].z - bv.corner.z);
            if (x < 0)
                continue;
            if (y < 0)
                continue;
            if (z < 0)
                continue;
            if (x > numCellsSide)
                continue;
            if (y > numCellsSide)
                continue;
            if (z > numCellsSide)
                continue;
            ++x;
            ++y;
            ++z;
            grid[x + numCellsSide * y + numCellsSide * numCellsSide * z].push_back(i);
        }
    }
}

void Grid::findCollisions(DistanceConstraintData &constraints, ParticleData &particles, bool &ignorePhase, bool parallel)
{
    if (parallel)
    {
        tbb::parallel_for(tbb::blocked_range<size_t>(0, grid.size()),
                [&](const tbb::blocked_range<size_t>& r) {
            for (size_t i = r.begin(); i != r.end(); ++i)
            {
                if (i % numCellsSide == 0)
                    continue;
                if (i % numCellsSide + 2 == 0)
                    continue;

                for (int x = -1; x < 2; x++) for (int y = -1; y < 2; y++) for (int z = -1; z < 2; z++)
                {
                    for (int j = 0; j < grid[i].size(); ++j)
                    {
                        int xyz = i + x + y * numCellsSide + z * numCellsSide * numCellsSide;
                        for (int k = i==xyz?j+1:0; k < grid[xyz].size(); ++k)
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
                                c.distance = particles.radius[idx1] + particles.radius[idx2];
                                particles.numBoundConstraints[idx1].fetch_and_increment();
                                particles.numBoundConstraints[idx2].fetch_and_increment();

                                addConstraint(constraints, c);
                            }
                        }
                    }
                }
            }
        });
    }
    else
    {
        for (int i = 0; i < grid.size(); ++i)
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
                            for (int k = i==xyz?j+1:0; k < grid[xyz].size(); ++k)
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
                                    c.distance = particles.radius[idx1] + particles.radius[idx2];
                                    particles.numBoundConstraints[idx1]++;
                                    particles.numBoundConstraints[idx2]++;

                                    addConstraint(constraints, c);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
