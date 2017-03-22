#pragma once

#include <glm/glm.hpp>

struct Intersection {
	glm::vec3 point;
	glm::vec3 responseGradient;
	float responseDistance;

	bool operator==(const Intersection &i) {
		//return p.pos == this->pos && p.velocity == this->velocity && p.invmass == this->invmass && p.phase == this->phase;
		return i.point == this->point
			&& i.responseGradient == this->responseGradient
			&& i.responseDistance == this->responseDistance;
	}
};
