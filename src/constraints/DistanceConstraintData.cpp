#include "DistanceConstraintData.h"



DistanceConstraintData::DistanceConstraintData()
{
    particles.reserve(MAX_DISTANCE_CONSTRAINTS);
    stiffness.reserve(MAX_DISTANCE_CONSTRAINTS);
    threshold.reserve(MAX_DISTANCE_CONSTRAINTS);
    distance.reserve(MAX_DISTANCE_CONSTRAINTS);
    equality.reserve(MAX_DISTANCE_CONSTRAINTS);
    cardinality=0;
}


DistanceConstraintData::~DistanceConstraintData()
{
}

float DistanceConstraintData::evaluate(int constraintIndex, ParticleData &particleData)
{
    int firstParticleIndex = particles[constraintIndex][0];
    int secondParticleIndex = particles[constraintIndex][1];

    glm::vec3 p1 = particleData.pPosition[firstParticleIndex];
    glm::vec3 p2 = particleData.pPosition[secondParticleIndex];

    return length(p1 - p2) - distance[constraintIndex];
}

glm::vec3 DistanceConstraintData::gradient(int constraintIndex, int particleIndex, ParticleData &particleData)
{
    int firstParticleIndex  = particles[constraintIndex][0];
    int secondParticleIndex = particles[constraintIndex][1];

    glm::vec3 p1 = particleData.pPosition[firstParticleIndex];
    glm::vec3 p2 = particleData.pPosition[secondParticleIndex];

    glm::vec3 c = (p1 - p2) / length(p1 - p2);

    return (particleIndex == firstParticleIndex ? c : -c);
}

float DistanceConstraintData::scaleFactor(int constraintIndex, ParticleData &particleData)
{
    int firstParticleIndex = particles[constraintIndex][0];
    int secondParticleIndex = particles[constraintIndex][1];

    glm::vec3 p1 = particleData.pPosition[firstParticleIndex];
    glm::vec3 p2 = particleData.pPosition[secondParticleIndex];

    std::vector<float> &invmass = particleData.invmass;

    return (length(p1 - p2) - distance[constraintIndex]) / (invmass[firstParticleIndex] + invmass[secondParticleIndex]);
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

    for (int i = cardinality-1; i >= 0; i--) {
        if (evaluate(i, particleData) > threshold[i]) {

            for (unsigned int p = 0; p < particles[i].size(); p++) {
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



void addConstraint(DistanceConstraintData &data, int firstParticleIndex, int secondParticleIndex, float stiffness, float distance, float threshold, bool equality)
{
    data.particles.push_back({ firstParticleIndex, secondParticleIndex });
    data.stiffness.push_back(stiffness);
    data.distance.push_back(distance);
    data.equality.push_back(equality);
    data.threshold.push_back(threshold);
    data.cardinality++;
}
