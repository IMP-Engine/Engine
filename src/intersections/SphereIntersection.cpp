#include "SphereIntersection.h"


bool intersect(vec3 &c1, float invMass1, vec3 &c2, float invMass2, Intersection &i) {
    vec3 delta = c1 - c2;
    float deltaLength = length(delta);

    if (deltaLength < PARTICLE_RADIUS_2) 
    {
        float diff = (deltaLength - PARTICLE_RADIUS_2) / (deltaLength * (invMass1 + invMass2));

        i.point    = -invMass1*delta*diff;
        i.responseGradient =  invMass2*delta*diff;
        return true;
    }
    return false;
}
