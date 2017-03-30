#pragma once
#include <glm/glm.hpp>

namespace model {
	// TODO Radius?
	struct modelConfig {
		float invmass;            // Particle mass
		glm::vec3 centerPos;      // Where to place model
		int phase;                // Particle phase
		float elasticity;          // Constraint elasticity
		float distanceThreshold;  // Constraind distance treshold
		glm::vec3 scale;          // Scale factor that model is scaled by (can be used to rotate)
		glm::ivec3 numParticles;    // Number of particles in each direction (used by predefined model)
	};
}
