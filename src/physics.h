#pragma once

#include <vector>
#include "particles/Particle.h"
#include "intersections.h"
#include "Scene.h"

namespace physics {
	void simulate(std::vector<Particle>* particles/*, std::vector<Constraint*>* constraints */,Scene *scene, float dt);
}
