#pragma once

#include <vector>
#include <stdio.h>
#include <algorithm>
#include <stdio.h>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/norm.hpp>

#include "../tbb/include/tbb/parallel_for.h"
#include "collision/collision.h"
#include "scenes/Scene.h"
#include "particles/ParticleData.h"
#include "constraints/ConstraintData.h"
#include "intersections.h"
#include "performance.h"
#include "devices/Device.h"
#include "devices/CPU.h"
#include "devices/GPU.h"

class Physics 
{
public:

    enum ParallelMode {
        sequential,
        multicore,
        GPGPU
    };

    enum ConstraintSolve {
        jacobi,
        gaussSeidel
    };

    Physics(std::vector<Triangle> & triangles);
    ~Physics() {};

    void gui();

    ParallelMode getMode();

    void setMode(ParallelMode newMode);

    bool doPhysics;
    float overRelaxConst;
    float pSleeping;
    float restitutionCoefficientN;
    float restitutionCoefficientT;
    bool parallelConstraintSolve;
    bool parallelDetectCollisions;
    float kineticFC; // kinetic friction coefficient
    float staticFC; // static friction coefficient

    float gravity;
    float wind;

    int iterations;
    int stabilizationIterations;

    ParticleData particles;
    ConstraintData constraints;
    std::vector<Triangle> triangles;

    Device* device;

    void step(float dt);

private:

    ParallelMode mode;
    ConstraintSolve solve;

};
