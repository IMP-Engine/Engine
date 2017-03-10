#include <vector>
#include "particles/Particle.h"
#include "constraints/Constraint.h"

namespace physics {
	void simulate(std::vector<Particle>* particles, std::vector<Constraint*>* constraints , float dt, int iterations);
}
