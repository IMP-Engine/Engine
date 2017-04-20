#pragma once
#include "modelConfig.h"
#include "../particles/ParticleData.h"
#include "../constraints/FixedPointConstraintData.h"
#include "../constraints/DistanceConstraintData.h"
#include "../constraints/ConstraintData.h"

namespace model {
    void makeClothModel(ModelConfig &config, bool hasFixedCorners, ParticleData &particles, ConstraintData &constraints);
}