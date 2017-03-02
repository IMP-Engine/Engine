#pragma once
#include "Constraint.h"
#include "particles/Particle.h"
#include <glm/geometric.hpp>

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
		vec3 p1 = this->particles[0]->pPos;
		vec3 p2 = this->particles[1]->pPos;
		
		return equality || length(p1 - p2) - distance < 0;
	}

	float evaluateScaleFactor() {
		vec3 p1 = this->particles[0]->pPos;
		vec3 p2 = this->particles[1]->pPos;

		return (length(p1 - p2) - distance) / (this->particles[0]->invmass + this->particles[1]->invmass);
	}

	vec3 evaluateGradient(std::vector<Particle*>::iterator p) {
		vec3 p1 = this->particles[0]->pPos;
		vec3 p2 = this->particles[1]->pPos;

		vec3 c = (p1 - p2) / length(p1 - p2);

		return (p == this->particles.begin() ? c : -c);
	}

};

