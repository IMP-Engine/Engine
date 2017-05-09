#pragma once

#ifndef _MODEL_CONFIG
#define _MODEL_CONFIG 1

#include <glm/glm.hpp>

namespace model {
    class ModelConfig {
    public:
        float invmass;            // Particle mass
        glm::vec3 centerPos;      // Where to place model
        int phase;                // Particle phase
        float stiffness;          // Constraint stiffness
        float distanceThreshold;  // Constraind distance treshold
        glm::vec3 scale;          // Scale factor that model is scaled by (can be used to rotate)
        glm::ivec3 numParticles;    // Number of particles in each direction (used by predefined model)

        void setDefaults() {
            centerPos = glm::vec3(0.f);
            distanceThreshold = 2;
            invmass = 0.8f;
            phase = 0;
            scale = glm::vec3(5.f, 1.f, 5.f);
            stiffness = 0.8f;
            numParticles = glm::ivec3(100, 1, 100);
        }
    };

}


#endif // !_MODEL_CONFIG
