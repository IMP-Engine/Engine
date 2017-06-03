#pragma once

#include <glm/common.hpp>

#include "../particles/ParticleData.h"
#include "../constraints/ConstraintData.h"
#include "../particles/ParticleRenderer.h"

/*********************************/
/* This class serves to function */
/* as an abstraction to on what  */
/* device the code is currently  */
/* being run. (CPU, GPU, etc.)   */
/*********************************/

class Device
{
public:
    
    Device(ParticleData & particles, ConstraintData & constraints, std::vector<Triangle> & triangles);

    // Particle movement
    virtual void verletIntegrate(float dt, glm::vec3 forces) = 0;
    virtual void solveConstraintsJacobi(int iterations, float omega) = 0;
    virtual void solveConstraintsGaussSeidel(int iterations, float omega) = 0;
    virtual void updateParticles(float dt, float pSleeping) = 0;

    // Collision detection
    virtual void detectCollisionsTriangles() = 0;
    virtual void detectCollisionParticles() = 0;
    virtual void stabilizationPass(int stabilizationIterations) = 0;

    virtual void dampCollision(float restitutionCoefficientT, float restitutionCoefficientN) = 0;

    // Doesn't belong to the responsibilities of a Device perhaps?
    // But is dependent on the way particles are stored (RAM/GPU)
    // Maybe move to physicssystem?
    virtual void renderParticles(glm::mat4 &modelViewProjectionMatrix, glm::mat4 &modelViewMatrix, glm::vec3 &viewSpaceLightPosition, glm::mat4 &projectionMatrix, int height) = 0;

protected:

    ParticleData & particles;
    ConstraintData & constraints;
    std::vector<Triangle> & triangles;
};