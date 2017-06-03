#include "gpu.h"
#include "Device.h"

Device::Device(ParticleData & particles, ConstraintData & constraints, std::vector<Triangle> & triangles)
    : particles(particles) , constraints(constraints), triangles(triangles)
{
}
