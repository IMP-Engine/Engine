#include "DistanceConstraintData.h"



DistanceConstraintData::DistanceConstraintData()
{
    particles.reserve(MAX_DISTANCE_CONSTRAINTS);
    elasticity.reserve(MAX_DISTANCE_CONSTRAINTS);
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
    int firstParticleIndex = particles[constraintIndex].x;
    int secondParticleIndex = particles[constraintIndex].y;

    glm::vec3 p1 = particleData.pPosition[firstParticleIndex];
    glm::vec3 p2 = particleData.pPosition[secondParticleIndex];

    return length(p1 - p2) - distance[constraintIndex];
}

glm::vec3 DistanceConstraintData::gradient(int constraintIndex, int particleIndex, ParticleData &particleData)
{
    int firstParticleIndex  = particles[constraintIndex].x;
    int secondParticleIndex = particles[constraintIndex].y;

    glm::vec3 p1 = particleData.pPosition[firstParticleIndex];
    glm::vec3 p2 = particleData.pPosition[secondParticleIndex];

    glm::vec3 c = (p1 - p2);
    c /= length(c);

    return (particleIndex == firstParticleIndex ? c : -c);
}

float DistanceConstraintData::scaleFactor(int constraintIndex, ParticleData &particleData, float lambda, double dt)
{
	float aDash = (elasticity[constraintIndex] / (dt*dt));

    int firstParticleIndex = particles[constraintIndex].x;
    int secondParticleIndex = particles[constraintIndex].y;

    glm::vec3 p1 = particleData.pPosition[firstParticleIndex];
    glm::vec3 p2 = particleData.pPosition[secondParticleIndex];

    std::vector<float> &invmass = particleData.invmass;

    return (length(p1 - p2) - distance[constraintIndex] + aDash*lambda) / (invmass[firstParticleIndex] + invmass[secondParticleIndex] + aDash);
}

void DistanceConstraintData::clear()
{
    particles.clear();
    elasticity.clear();
    distance.clear();
    equality.clear();
    threshold.clear();
    cardinality = 0;
}

void DistanceConstraintData::removeBroken(ParticleData &particleData)
{

    for (int i = cardinality-1; i >= 0; i--) {
        if (evaluate(i, particleData) > threshold[i]) {

            for (unsigned int p = 0; p < 2; p++) {
                particleData.numBoundConstraints[particles[i][p]]--;
            }

            particles.erase(particles.begin() + i);
            elasticity.erase(elasticity.begin() + i);
            distance.erase( distance.begin()  + i);
            equality.erase( equality.begin()  + i);
            threshold.erase(threshold.begin() + i);
            cardinality--;
        }

    }
}



void addConstraint(DistanceConstraintData &data, DistanceConstraint &config)
{
    data.particles.push_back({ config.firstParticleIndex, config.secondParticleIndex });
    data.elasticity.push_back(config.elasticity);
    data.distance.push_back(config.distance);
    data.equality.push_back(config.equality);
    data.threshold.push_back(config.threshold);
    data.cardinality++;
}
