#pragma once
#include "Constraint.h"
#include "particles/Particle.h"
#include <glm/vec3.hpp>

class VolumeConstraint :
	public Constraint
{
public:
	float volume;

	VolumeConstraint(Particle* p1, Particle* p2, float elasticity, float volume, bool equality = true) {
		this->particles.push_back(p1);
		this->particles.push_back(p2);
		this->elasticity = elasticity;
		this->volume = volume;
		this->equality = equality;
	}

	bool evaluate() {
		return equality || 1 < 0;
	}

	float evaluateScaleFactor() {
		return 0.0f;
	}

	vec3 evaluateGradient(std::vector<Particle*>::iterator p) {
		return vec3();
	}

};

