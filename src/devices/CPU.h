#pragma once

#include "Device.h"
#include <glm/common.hpp>
#include "../tbb/include/tbb/parallel_for.h"
#include "../collision/collision.h"
#include "../particles/ParticleRenderer.h"

class CPU : public Device
{
public:

    CPU(ParticleData & particles, ConstraintData & constraints, std::vector<Triangle> & triangles, bool parallel);

private:

    bool parallel;
    ParticleRenderer particleRenderer;

    virtual void verletIntegrate(float dt, glm::vec3 forces) override;

    virtual void solveConstraintsJacobi(int iterations, float omega) override;

    virtual void solveConstraintsGaussSeidel(int iterations, float omega) override;

    virtual void updateParticles(float dt, float pSleeping) override;

    virtual void detectCollisionsTriangles() override;

    virtual void detectCollisionParticles() override;

    virtual void stabilizationPass(int stabilizationIterations) override;

    virtual void dampCollision(float restitutionCoefficientT, float restitutionCoefficientN) override;

    virtual void renderParticles(glm::mat4 &modelViewProjectionMatrix, glm::mat4 &modelViewMatrix, glm::vec3 &viewSpaceLightPosition, glm::mat4 &projectionMatrix, int height) override;

    virtual void stop(ParticleData & particles, ConstraintData & constraints) override;

    virtual void start(ParticleData & particles, ConstraintData & constraints) override;

};