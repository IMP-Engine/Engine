#pragma once

#include <vector>
#include "glm/glm.hpp"
#include "../scenes/Scene.h"
#include "../intersections.h"

#include "../particles/ParticleData.h"
#include "../tbb/include/tbb/parallel_for.h"
#include "../../tbb/include/tbb/mutex.h"

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
};
