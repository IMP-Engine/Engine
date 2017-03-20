#pragma once

#include <vector>
#include "../particles/Particle.h"
#include "../constraints/Constraint.h"
#include "modelConfig.h"
#include "model.h"


namespace Box {
	void makeBox(std::vector<Particle>* particles, std::vector<Constraint*>* constraints, model::modelConfig config);
}

