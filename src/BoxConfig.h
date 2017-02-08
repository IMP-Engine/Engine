#pragma once
#include "linmath\float3.h"
#include "linmath\int3.h"

using namespace chag;

struct BoxConfig {
	float mass;
	float3 center_pos, dimensions;
	int3 num_particles;
};