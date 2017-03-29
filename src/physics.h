#pragma once

#include <vector>
#include "particles/Particle.h"
#include "scenes/Scene.h"
#include "intersections.h"
#include "constraints/Constraint.h"

namespace physics {
	void simulate(std::vector<Particle> &particles, std::vector<Constraint*> &constraints, Scene *scene, float dt, int iterations);
}
