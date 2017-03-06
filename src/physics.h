#pragma once

#include <vector>
#include "particles/Particle.h"
#include "Scene.h"
#include "Constraint.h"
#include "intersections.h"

namespace physics {
	void simulate(std::vector<Particle> &particles, std::vector<Constraint*> &constraints, Scene *scene, float dt, int iterations);
}
