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



Box *make_box(BoxConfig * const config) {
    Box *box = new Box;
    int num_particles = config->num_particles.x * config->num_particles.y * config->num_particles.z;

    //box->particles.resize(num_particles);

    float dx, dy, dz;

    if (config->num_particles.x > 1) {
        dx = config->dimensions.x / (config->num_particles.x-1);
    }
    else if (config->num_particles.x == 1) {
        dx = 0;
    }
    if (config->num_particles.y > 1) {
        dy = config->dimensions.y / (config->num_particles.y-1);
    }
    else if (config->num_particles.y == 1) {
        dy = 0;
    }
    if (config->num_particles.z > 1) {
        dz = config->dimensions.z / (config->num_particles.z-1);
    }
    else if (config->num_particles.z == 1) {
        dz = 0;
    }

    // Get the "minimum" corner to use as base position
    vec3 base_pos = config->center_pos - (config->dimensions * 0.5f);

    box->dimensions = config->dimensions;
    box->center_pos = config->center_pos;
    box->mass = config->mass;
    box->num_particles = config->num_particles;

    // Create particles
    for (int i = 0; i < config->num_particles.x; i++) {
        for (int j = 0; j < config->num_particles.y; j++) {
            for (int k = 0; k < config->num_particles.z; k++) {
                Particle p = Particle();

                // Offset from base
                p.pos = base_pos + vec3(i*dx, j*dy, k*dz);

                // Uniform distribution of mass
                p.invmass = config->mass / num_particles;
                p.velocity = vec3(0.f, 0.f, 0.f);
                p.phase = config->phase;

                box->particles.push_back(p);
            }
        }
    }

    // Create constraints
    std::vector<std::pair<int, int>> con;
    float stiffness = config->stiffness;
    float distanceThreshold = config->distanceThreshold;
    for (int i = 0; i < box->particles.size(); i++) {
        for (int j = 0; j < box->particles.size(); j++) {
            if (j != i && glm::distance(box->particles[i].pos, box->particles[j].pos) <= sqrt(dx*dx + dy*dy + dz*dz) && std::find(con.begin(), con.end(), std::pair<int,int>(j,i)) == con.end())
            {
                con.push_back(std::pair<int, int>(i, j));
                Constraint* c = new DistanceConstraint(
                        &box->particles[i],
                        &box->particles[j],
                        stiffness,
                        distanceThreshold,
                        glm::distance(box->particles[i].pos,
                        box->particles[j].pos));
                box->constraints.push_back(c);
                box->particles[i].numBoundConstraints += 1;
                box->particles[j].numBoundConstraints += 1;
            }
        }
    }
    return box;
}
