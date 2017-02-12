#pragma once
#include "Constraint.h"
#include "Particle.h"
#include <cmath>

class BendingConstraint :
	public Constraint
{
public:
	float angle;

	BendingConstraint(Particle* p1, Particle* p2, Particle* p3, Particle* p4, float stiffness, float angle, bool equality = true) {
		this->particles.push_back(p1);
		this->particles.push_back(p2);
		this->particles.push_back(p3);
		this->particles.push_back(p4);
		this->stiffness = stiffness;
		this->angle = angle;
		this->equality = equality;
	}

	bool evaluate() {
			return equality || (acos(calcDotNormals()) - this->angle) < 0;
	}
	//TODO
	float evaluateScaleFactor() {
		float accum = acos(calcDotNormals()) - this->angle;
		for (std::vector<Particle*>::iterator it = this->particles.begin(); it != this->particles.end(); it++)
		{
			accum += (*it)->invmass + lengthSquared(evaluateGradient(it));
		}
	}

	// TODO
	float3 evaluateGradient(std::vector<Particle*>::iterator p) {
		float d = calcDotNormals();
		switch (p - this->particles.begin)
		{
		case 0:

			break;
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		default:
			// hmmm
		}
	}

private:
	// Dot product of the normals of two contigious triangle sections
	float calcDotNormals() {
		const float3 p1 = this->particles[0]->pos;
		const float3 p2 = this->particles[1]->pos;
		const float3 p3 = this->particles[2]->pos;
		const float3 p4 = this->particles[3]->pos;
		const float3 v1 = cross(p2 - p1, p3 - p1);
		const float3 v2 = cross(p2 - p1, p4 - p1);

		return dot(normalize(v1), normalize(v2));
	}
};

