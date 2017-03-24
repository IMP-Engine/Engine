#pragma once

#include <vector>
#include "../particles/Particle.h"
#include "../particles/ParticleData.h"
#include "../constraints/Constraint.h"
#include "modelConfig.h"
#include "model.h"


namespace Box {
	void makeBox(ParticleData &particles, ConstraintData &constraints, model::modelConfig config);
}

