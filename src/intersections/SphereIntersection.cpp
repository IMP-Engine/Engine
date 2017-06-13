#include "SphereIntersection.h"

bool intersect(ParticleData &particles, int first, int second, Intersection &i) {
    /**
     * Aliases 
     */
    std::vector<vec3> &pPos = particles.pPosition;
    std::vector<float> &invmass = particles.invmass;
    std::vector<float> &radius = particles.radius;


    vec3 delta = pPos[first] - pPos[second];
    float deltaLength = glm::length2(delta);
    float totalRadius = radius[first] + radius[second];
    

    return (deltaLength < totalRadius*totalRadius);
}
