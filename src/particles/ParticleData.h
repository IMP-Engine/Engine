#pragma once
#include <vector>
#include "../tbb/include/tbb/atomic.h"
#include "glm/glm.hpp"
#include "Particle.h"


#define MAX_PARTICLES 200000

class ParticleData
{
public:
    ParticleData();
    ~ParticleData();
    unsigned int cardinality;

    std::vector<glm::vec3> position;
    std::vector<glm::vec3> pPosition;
    std::vector<glm::vec3> velocity;
    std::vector<float> invmass;
    std::vector<int> phase;
    std::vector<tbb::atomic<int>> numBoundConstraints;
    std::vector<float> radius;


    void clear();
};

void addParticle(Particle &p, ParticleData &data);
