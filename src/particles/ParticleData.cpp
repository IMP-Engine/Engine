#include "ParticleData.h"



ParticleData::ParticleData()
{
    cardinality = 0;
    invmass.reserve(MAX_PARTICLES);
    phase.reserve(MAX_PARTICLES);
    position.reserve(MAX_PARTICLES);
    pPosition.reserve(MAX_PARTICLES);
    radius.reserve(MAX_PARTICLES);
    velocity.reserve(MAX_PARTICLES);
    numBoundConstraints.reserve(MAX_PARTICLES);
}


ParticleData::~ParticleData()
{
}


void ParticleData::clear() {
    position.clear();
    pPosition.clear();
    velocity.clear();
    invmass.clear();
    phase.clear();
    numBoundConstraints.clear();
    radius.clear();
    cardinality = 0;
}

void addParticle(Particle &p, ParticleData &data) {
    data.position.push_back(p.pos);
    data.pPosition.push_back(p.pPos);
    data.velocity.push_back(p.velocity);
    data.invmass.push_back(p.invmass);
    data.phase.push_back(p.phase);
    data.numBoundConstraints.push_back(p.numBoundConstraints);
    data.radius.push_back(p.radius);
    data.cardinality++;
}