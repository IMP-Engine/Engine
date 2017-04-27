#pragma once

#include <glm/gtx/norm.hpp>

#include "../primitives.h"
#include "Intersection.h"

bool intersect(ParticleData &particles, int first, int second, Intersection &i);

