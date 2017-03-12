#pragma once

#include <glm/glm.hpp>

struct Intersection {
	glm::vec3 point;
	glm::vec3 responseGradient;
	float responseDistance;
};
