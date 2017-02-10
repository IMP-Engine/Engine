#pragma once
#include "Constraint.h"
#include "Particle.h"

class DistanceConstraint :
	public Constraint
{
public:
	float distance;

	DistanceConstraint(Particle* p1, Particle* p2, float stiffness, float distance, bool equality = true) {
		this->particles.push_back(p1);
		this->particles.push_back(p2);
		this->stiffness = stiffness;
		this->distance = distance;
		this->equality = equality;
	}

	bool evaluate() {
		float3 p1 = this->particles[0]->pos;
		float3 p2 = this->particles[1]->pos;
		 
		return equality || length(p1 - p2) - distance < 0;
	}

	float evaluateScaleFactor() {
		float3 p1 = this->particles[0]->pos;
		float3 p2 = this->particles[1]->pos;

		return (length(p1 - p2) - distance) / (this->particles[0]->invmass + this->particles[1]->invmass);
	}

	float3 evaluateGradient(std::vector<Particle*>::iterator p) {
		float3 p1 = this->particles[0]->pos;
		float3 p2 = this->particles[1]->pos;

		float3 c = (p1 - p2) / length(p1 - p2);

		return (p == this->particles.begin() ? c : -c); // TODO typo? Where should '-' be?
	}

};

