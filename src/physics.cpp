#include "physics.h"

#ifndef WORLD_MIN
#define WORLD_MIN vec3(-20.f,-20.f,-20.f)
#define WORLD_MAX vec3( 20.f, 20.f, 20.f)
#endif // !WORLD_MIN

Physics::Physics(std::vector<Triangle> & triangles)
    : mode(ParallelMode::sequential), iterations(4), stabilizationIterations(2), pSleeping(0.0001f),
    overRelaxConst(1.f), restitutionCoefficientT(0.8f), restitutionCoefficientN(0.8f),
    kineticFC(0.2f), staticFC(0.2f), gravity(4.f), wind(0.f), doPhysics(false), triangles(triangles),
    solve(ConstraintSolve::jacobi)
{
    device = new CPU(particles, constraints, triangles, false);
}

void Physics::gui() {
    ImGui::Checkbox("Physics", &doPhysics);

    ParallelMode mode = this->getMode();
    ImGui::RadioButton("Sequential", reinterpret_cast<int*>(&mode), static_cast<int>(sequential)); ImGui::SameLine();
    ImGui::RadioButton("Multicore", reinterpret_cast<int*>(&mode), static_cast<int>(multicore)); ImGui::SameLine();
    ImGui::RadioButton("GPGPU", reinterpret_cast<int*>(&mode), static_cast<int>(GPGPU));
    this->setMode(mode);

    ImGui::RadioButton("Jacobi", reinterpret_cast<int*>(&solve), static_cast<int>(jacobi)); ImGui::SameLine();
    ImGui::RadioButton("Gauss-Seidel", reinterpret_cast<int*>(&solve), static_cast<int>(gaussSeidel));

    ImGui::SliderInt("Solver Iterations", &iterations, 1, 32);
    ImGui::SliderInt("Collision Stabilization Iterations", &stabilizationIterations, 0, 32);
    ImGui::SliderFloat("Over-relax-constant", &overRelaxConst, 1, 5);
    ImGui::SliderFloat("Particle Sleeping (squared)", &pSleeping, 0, 1, "%.9f", 10.f);
    ImGui::SliderFloat("Tangential COR", &restitutionCoefficientT, -1, 1);
    ImGui::SliderFloat("Normal COR", &restitutionCoefficientN, 0, 1);
    ImGui::SliderFloat("Kinetic Friction Coefficient", &kineticFC, 0, 1);
    ImGui::SliderFloat("Static Friction Coefficient", &staticFC, 0, 1);
    ImGui::SliderFloat("Gravity", &gravity, 0, 10);
    ImGui::SliderFloat("Wind strength", &wind, 0, 10); 
}

Physics::ParallelMode Physics::getMode() { return mode; }

void Physics::setMode(ParallelMode newMode) {
    if (mode == newMode) return;

    //TODO download particle and constraintdata

    switch (mode) {
    case sequential:  break;
    case multicore:  break;
    //case GPGPU: GPU->stop(particles); break;
    }

    switch (newMode) {
    case sequential: device = new CPU(particles, constraints, triangles, false); break;
    case multicore: device = new CPU(particles, constraints, triangles, true); break;
    //case GPGPU: GPU->start(particles, constraints); break;
    }

    mode = newMode;
}

void Physics::step(float dt)
{
    if (!doPhysics) return;

    /* Aliases */
    std::vector<vec3>  &position  = particles.position;
    std::vector<vec3>  &pPosition = particles.pPosition;
    std::vector<vec3>  &velocity  = particles.velocity;
    std::vector<float> &invmass   = particles.invmass;
    std::vector<int>   &phase     = particles.phase;
    std::vector<tbb::atomic<int>>   &numBoundConstraints = particles.numBoundConstraints;

    device->verletIntegrate(dt, glm::vec3(wind, gravity, 0.f));

    /*// Breakable constraints
    int id = performance::startTimer("Remove broken constraints");
    constraints.removeBroken(particles);
    performance::stopTimer(id);
    */

    int id = performance::startTimer("Scene collision detection");
    device->detectCollisionsTriangles();
    performance::stopTimer(id);

    id = performance::startTimer("Detect collisions");
    device->detectCollisionParticles();
    performance::stopTimer(id);

    id = performance::startTimer("Solve collisions");
    device->stabilizationPass(stabilizationIterations);
    performance::stopTimer(id);


    id = performance::startTimer("Solve constraints");
    if (solve == jacobi)
        device->solveConstraintsJacobi(iterations, overRelaxConst);
    else if (solve == gaussSeidel)
        device->solveConstraintsGaussSeidel(iterations, overRelaxConst);
    performance::stopTimer(id);

    device->updateParticles(dt, pSleeping);

    id = performance::startTimer("Damp collisions");
    // Update velocities according to friction and restitution coefficients
    device->dampCollision(restitutionCoefficientT, restitutionCoefficientN);
    performance::stopTimer(id);
}

