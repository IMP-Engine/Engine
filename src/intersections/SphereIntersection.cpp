#include "SphereIntersection.h"


bool intersect(ParticleData particles, int first, int second, Intersection &i) {
    /**
     * Aliases 
     */
    std::vector<vec3> &pPos = particles.pPosition;
    std::vector<float> &invmass = particles.invmass;
    std::vector<float> &radius = particles.radius;


    vec3 delta = pPos[first] - pPos[second];
    float deltaLength = length(delta);
	float totalRadius = radius[first] + radius[second];


    if (deltaLength < totalRadius) 
    {
        float diff = (deltaLength - totalRadius) / (deltaLength * (invmass[first] + invmass[second]));

        i.point            = -invmass[first]  * delta * diff;
        i.responseGradient =  invmass[second] * delta * diff;
        return true;
    }
    return false;
}
