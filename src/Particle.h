#pragma once

#include "linmath/float3.h"

using namespace chag;

struct Particle
{
	float3 pos;
	float3 velocity;
	float invmass;
	int phase;
};

