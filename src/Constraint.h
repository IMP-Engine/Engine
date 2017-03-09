#pragma once
#include "particles/Particle.h"
#include <vector>
#include <glm/vec3.hpp>

using namespace glm;

class Constraint
{
public:

	/* 
	 * List of all particles related to a given constaint. 
	 */
	std::vector<Particle*> particles;
	
	/* 
	 * The stiffness associated with a specific constraint.
	 * The value should be in the interval [0,1]
	 */
	float stiffness;
	
	/* 
	 * Is this constraint of type C(P) = 0 or C(P) => 0
	 */
	bool equality;
	
	/* 
	 * Is the constraint fulfilled?
	 * The constraint will not be fulfilled even if the constraint evaluates to zero.
	 */
	virtual float evaluate() = 0;
	
	/*
	 * C(P1,...) / Sum_j w_j |grad_j C(P1,...)|^2 
	 * This method will not produce a correct result unless
	 * evaluate() has been called for the current position.
	 */
	virtual float evaluateScaleFactor() = 0;
	
	/*
	 * Evaluate the gradient of this constraint with regards to Particle p.
	 * This method might not work properly unless evaluate() has been called,
	 * consult derived constraint for more information.
	 */
	virtual vec3 evaluateGradient(std::vector<Particle*>::iterator p) = 0;

	/*
	 * Threshold for when constraint breaks.
	 */
	float threshold;
};
