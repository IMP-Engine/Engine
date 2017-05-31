#include "gpu.h"
#include "Device.h"

Device::Device(ParticleData particles, ConstraintData constraints)
{
    this->particles = particles;
    this->constraints = constraints;
}
