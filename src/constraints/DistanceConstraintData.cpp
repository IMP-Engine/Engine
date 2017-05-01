#include "DistanceConstraintData.h"

tbb::mutex mutex;

DistanceConstraintData::DistanceConstraintData()
{
    particles.reserve(MAX_DISTANCE_CONSTRAINTS);
    stiffness.reserve(MAX_DISTANCE_CONSTRAINTS);
    threshold.reserve(MAX_DISTANCE_CONSTRAINTS);
    distance.reserve(MAX_DISTANCE_CONSTRAINTS);
    equality.reserve(MAX_DISTANCE_CONSTRAINTS);
    cardinality=0;
}


DistanceConstraintData::~DistanceConstraintData(){}

float DistanceConstraintData::evaluate(int constraintIndex, ParticleData &particleData)
{
    int firstParticleIndex = particles[constraintIndex].x;
    int secondParticleIndex = particles[constraintIndex].y;

    glm::vec3 p1 = particleData.pPosition[firstParticleIndex];
    glm::vec3 p2 = particleData.pPosition[secondParticleIndex];

    return length(p1 - p2) - distance[constraintIndex];
}

bool DistanceConstraintData::solveDistanceConstraint(glm::vec3 & delta1, glm::vec3 & delta2, int constraintIndex, ParticleData & particleData, bool stabilize)
{
    int firstParticleIndex = particles[constraintIndex].x;
    int secondParticleIndex = particles[constraintIndex].y;

    glm::vec3 p1 = particleData.pPosition[firstParticleIndex];
    glm::vec3 p2 = particleData.pPosition[secondParticleIndex];

    vec3 diff = p1 - p2;

    float c = length(diff) - distance[constraintIndex];

    if (!equality[constraintIndex] && c >= 0)
        return false;

    if (stabilize)
    {   // Calculate delta with position instead of pPosition
        p1 = particleData.position[firstParticleIndex];
        p2 = particleData.position[secondParticleIndex];

        diff = p1 - p2;

        c = length(diff) - distance[constraintIndex];
    }

    vec3 grad = glm::normalize(diff);
    grad /= particleData.invmass[firstParticleIndex] + particleData.invmass[secondParticleIndex];
    
    delta1 = particleData.invmass[firstParticleIndex] * c * grad;
    delta2 = -particleData.invmass[secondParticleIndex] * c * grad;

    return true;
}

void DistanceConstraintData::clear()
{
    particles.clear();
    stiffness.clear();
    distance.clear();
    equality.clear();
    threshold.clear();
    cardinality = 0;
}

void DistanceConstraintData::removeBroken(ParticleData &particleData)
{
    for (int i = cardinality-1; i >= 0; i--)
    {
        if (evaluate(i, particleData) > threshold[i])
        {
            for (unsigned int p = 0; p < 2; p++)
            {
                particleData.numBoundConstraints[particles[i][p]]--;
            }

            particles.erase(particles.begin() + i);
            stiffness.erase(stiffness.begin() + i);
            distance.erase( distance.begin()  + i);
            equality.erase( equality.begin()  + i);
            threshold.erase(threshold.begin() + i);
            cardinality--;
        }
    }
}



void addConstraint(DistanceConstraintData &data, DistanceConstraint &config)
{
    mutex.lock();
    data.particles.push_back({ config.firstParticleIndex, config.secondParticleIndex });
    data.stiffness.push_back(config.stiffness);
    data.distance.push_back(config.distance);
    data.equality.push_back(config.equality);
    data.threshold.push_back(config.threshold);
    data.cardinality++;
    mutex.unlock();
}
