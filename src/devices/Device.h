#pragma once

#include <glm/vec3.hpp>

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
    
    Device(ParticleData particles, ConstraintData constraints);

    // Particle movement
    virtual void verletIntegrate(float dt, glm::vec3 forces) = 0;
    virtual void solveConstraintsJacobi() = 0;
    virtual void solveConstraintsGaussSeidel() = 0;
    virtual void updateParticlePositions() = 0;
    virtual void updateParticleVelocities(float dt) = 0;

    // Collision detection
    virtual void detectCollisions() = 0;
    virtual void createCollisionConstraints() = 0;
    virtual void removeCollisionConstraints() = 0;
    virtual void stabilizationPass() = 0;

    virtual void dampCollision() = 0;

    // Doesn't belong to the responsibilities of a Device perhaps?
    // But is dependent on the way particles are stored (RAM/GPU)
    virtual void renderParticles() = 0;

private:

    ParticleData particles;
    ConstraintData constraints;

    ParticleRenderer renderer;
};