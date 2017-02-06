#pragma once
#include "BoxConfig.h"
#include "Particle.h"

class Box
{
public:
	Box();
	~Box();
	BoxConfig config;
	Particle particles[];
};


Box *make_box(BoxConfig * const config);

