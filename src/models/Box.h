#pragma once

#include <vector>
#include "../particles/ParticleData.h"
#include "../constraints/ConstraintData.h"
#include "modelConfig.h"
#include "model.h"


namespace Box {
    void makeBox(ParticleData &particles, ConstraintData &constraints, model::ModelConfig config);
}

