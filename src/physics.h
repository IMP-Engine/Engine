#pragma once

#include <vector>
#include <stdio.h>
#include <algorithm>
#include <stdio.h>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/norm.hpp>

#include "scenes/Scene.h"
#include "particles/ParticleData.h"
#include "constraints/ConstraintData.h"
#include "intersections.h"
#include "performance.h"


class Physics 
{
public:
    Physics() {};
    ~Physics() {};

    bool useGS;
    float sumC;
    int iter;

    float overRelaxConst;
    float pSleeping;
    float restitutionCoefficientN;
    float restitutionCoefficientT;

    int iterations;
	int collisionIterations;

    ParticleData particles;
    ConstraintData constraints;

	void step(Scene *scene, float dt, bool &isRunning);

private:
    /*
        Stationary iterative linear solver - Gauss-Seidel
    */
    void resolveConstraints(std::vector<glm::vec3> & pPosition, std::vector<float> & invmass, std::vector<int> & numBoundConstraints, bool &isRunning);

    void dampPlaneCollision(std::vector<int> & numBoundConstraints, std::vector<glm::vec3> & velocity, PlaneCollisionConstraintData & triangleConstraints);

    /*
        Project position and predicted positions to valid states as specified by the planeCollisions constraints
    */
    void resolveCollisons(std::vector<glm::vec3> & position, std::vector<glm::vec3> & pPosition, PlaneCollisionConstraintData & planeConstraints, DistanceConstraintData & particleConstraints);

    /*
        If there is a collision with the scene, add that constraint to triangleConstraints.
    */
    void detectCollisions(Scene * scene, std::vector<int> & numBoundConstraints, PlaneCollisionConstraintData & triangleConstraints, std::vector<int> & phase, std::vector<glm::vec3> & pPosition);
};
