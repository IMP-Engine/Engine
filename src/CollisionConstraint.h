#pragma once
#include "Constraint.h"
#include "Particle.h"

class CollisionConstraint :
	public Constraint
{
public:
	float thickness;
	bool qAbove;

	CollisionConstraint(Particle* q, Particle* p1, Particle* p2, Particle* p3, float stiffness, float thickness, bool qAbove, bool equality = true) {
		this->particles.push_back(q);
		this->particles.push_back(p1);
		this->particles.push_back(p2);
		this->particles.push_back(p3);
		this->stiffness = stiffness;
		this->thickness = thickness;
		this->equality = equality;
		this->qAbove = qAbove;
	}

	bool evaluate() {
		float3 q  = this->particles[0]->pos;
		float3 p1 = this->particles[1]->pos;
		float3 p2 = this->particles[2]->pos;
		float3 p3 = this->particles[3]->pos;

		if (qAbove) return equality || dot(q - p1, normalize(cross(p2 - p1, p3 - p1))) < 0;
		else return equality || dot(q - p1, normalize(cross(p3 - p1, p2 - p1))) < 0;
	}

	// TODO
	float evaluateScaleFactor() {
		return 0.0f;
	}

	// TODO
	float3 evaluateGradient(std::vector<Particle*>::iterator p) {
		return float3();
	}

};

