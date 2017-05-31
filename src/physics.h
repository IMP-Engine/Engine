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
#include "GPU/GPU.h"

class Physics 
{
public:

    enum Mode {
        sequential,
        multicore,
        GPGPU
    };

    Physics(std::vector<Triangle> triangles);
    ~Physics() {};

    void gui();

    Mode getMode();

    void setMode(Mode newMode);

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
    bool windActive;

    int iterations;
    int stabilizationIterations;

    ParticleData particles;
    ConstraintData constraints;
    std::vector<Triangle> triangles;

    GPU * GPU;

    void step(Scene *scene, float dt);

private:

    Mode mode;

    /*
        Stationary iterative linear solver - Gauss-Seidel
    */
    void resolveConstraints(std::vector<glm::vec3> & position, std::vector<glm::vec3> & pPosition, std::vector<float> & invmass, std::vector<tbb::atomic<int>> & numBoundConstraints, PlaneCollisionConstraintData & planeConstraints, DistanceConstraintData & particleConstraints);

    void dampPlaneCollision(std::vector<tbb::atomic<int>> & numBoundConstraints, std::vector<glm::vec3> & velocity, PlaneCollisionConstraintData & triangleConstraints);

    /*
        Project position and predicted positions to valid states as specified by the planeCollisions constraints
    */
    void resolveCollisions(std::vector<glm::vec3> & position, std::vector<glm::vec3> & pPosition, std::vector<glm::float32> & invmass, PlaneCollisionConstraintData & planeConstraints, DistanceConstraintData & particleConstraints);

    /*
        If there is a collision with the scene, add that constraint to triangleConstraints.
    */
    void detectCollisions(Scene * scene, std::vector<tbb::atomic<int>> & numBoundConstraints, PlaneCollisionConstraintData & triangleConstraints, std::vector<int> & phase, std::vector<glm::vec3> & pPosition);
};
