#include "Box.h"
#include "../constraints/DistanceConstraint.h"
#include <algorithm>
#include <math.h>
#include <float.h>
#include <cfloat>

Box::Box()
{
}


Box::~Box()
{
    for (auto c : this->constraints) {
        delete c;
    }
}


Box *make_box(BoxConfig * const config, std::vector<Particle> &particleStorage) {
    Box *box = new Box;
    int numParticles = config->numParticles.x * config->numParticles.y * config->numParticles.z;

    //box->particles.resize(numParticles);

    float dx, dy, dz;

    if (config->numParticles.x > 1) {
        dx = config->dimensions.x / (config->numParticles.x-1);
    }
    else if (config->numParticles.x == 1) {
        dx = 0;
    }
    if (config->numParticles.y > 1) {
        dy = config->dimensions.y / (config->numParticles.y-1);
    }
    else if (config->numParticles.y == 1) {
        dy = 0;
    }
    if (config->numParticles.z > 1) {
        dz = config->dimensions.z / (config->numParticles.z-1);
    }
    else if (config->numParticles.z == 1) {
        dz = 0;
    }

    // Get the "minimum" corner to use as base position
    vec3 base_pos = config->center_pos - (config->dimensions * 0.5f);

    box->dimensions = config->dimensions;
    box->center_pos = config->center_pos;
    box->mass = config->mass;
    box->numParticles = config->numParticles;

    // Create particles
    for (int i = 0; i < config->numParticles.x; i++) {
        for (int j = 0; j < config->numParticles.y; j++) {
            for (int k = 0; k < config->numParticles.z; k++) {
                Particle p = Particle();

                // Offset from base
                p.pos = base_pos + vec3(i*dx, j*dy, k*dz);

                // Uniform distribution of mass
                p.invmass = config->mass / numParticles;
                p.velocity = vec3(0.f, 0.f, 0.f);
                p.phase = config->phase;

                particleStorage.push_back(p);
                box->particles.push_back(&particleStorage[particleStorage.size() - 1]);


            }
        }
    }

    // Create constraints
    std::vector<std::pair<int, int>> con;
    float stiffness = config->stiffness;
    float distanceThreshold = config->distanceThreshold;
    for (unsigned int i = 0; i < box->particles.size(); i++) {
        for (unsigned int j = 0; j < box->particles.size(); j++) {
            if (j != i && glm::distance(box->particles[i]->pos, box->particles[j]->pos) <= sqrt(dx*dx + dy*dy + dz*dz) && std::find(con.begin(), con.end(), std::pair<int,int>(j,i)) == con.end())
            {
                con.push_back(std::pair<int, int>(i, j));
                Constraint* c = new DistanceConstraint(
                        box->particles[i],
                        box->particles[j],
                        stiffness,
                        distanceThreshold,
                        glm::distance(
                            box->particles[i]->pos,
                            box->particles[j]->pos
                        )
                );
                box->constraints.push_back(c);
                box->particles[i]->numBoundConstraints++;
                box->particles[j]->numBoundConstraints++;
            }
        }
    }
    return box;
}
