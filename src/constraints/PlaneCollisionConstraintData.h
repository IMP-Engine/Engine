#pragma once

#include <vector>
#include "glm/glm.hpp"
#include "../scenes/Scene.h"
#include "../intersections.h"

#include "../particles/ParticleData.h"
#include "../tbb/include/tbb/parallel_for.h"

#ifndef MAX_DISTANCE_CONSTRAINTS
#define MAX_DISTANCE_CONSTRAINTS 5000
#endif // !MAX_DISTANCE_CONSTRAINTS


struct PlaneCollisionConstraint {
    int particleIndex;
    vec3 normal;
    float distance;
};

class PlaneCollisionConstraintData
{
public:
    PlaneCollisionConstraintData();

    int cardinality;

    std::vector<unsigned int> particles;
    std::vector<glm::vec3> normals;
    std::vector<float> distances;

    bool solvePlaneCollision(glm::vec3 &delta, const int idx, ParticleData &particleData, bool stabilize);

    void addConstraint(PlaneCollisionConstraint &config);

    void clear();

    /*
    Used for parallelizing plane collision detection
    */
    class PlaneCollisionDetection
    {
    public:
        Scene * scene;
        ParticleData & particles;
        PlaneCollisionConstraintData & planeConstraints;

        void operator()(const tbb::blocked_range<size_t>& r) const {
            std::vector<tbb::atomic<int>> &numBoundConstraints = particles.numBoundConstraints;

            for (size_t particleIndex = r.begin(); particleIndex != r.end(); ++particleIndex)
            {
                // Check collisions with scene
                for (std::vector<Triangle>::iterator t = scene->triangles.begin(); t != scene->triangles.end(); ++t)
                {
                    Intersection isect;
                    if (intersect((*t), particles, particleIndex, isect))
                    {
                        PlaneCollisionConstraint c;
                        c.particleIndex = particleIndex;
                        c.normal = isect.responseGradient;
                        c.distance = glm::dot(t->v0, isect.responseGradient) + particles.radius[particleIndex];
                        ++numBoundConstraints[particleIndex];

                        planeConstraints.addConstraint(c);
                    }
                }
            }
        }

        PlaneCollisionDetection(Scene * a, ParticleData & b, PlaneCollisionConstraintData & c) :
            scene(a), particles(b), planeConstraints(c)
        {}
    };
};
