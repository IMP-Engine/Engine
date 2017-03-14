#include "SphereIntersection.h"


bool intersect(Particle &p1, Particle &p2, Intersection &i) {
    vec3 delta = p1.pPos - p2.pPos;
    float deltaLength = length(delta);
	float totalRadius = p1.radius + p2.radius;


    if (deltaLength < totalRadius) 
    {
        float diff = (deltaLength - totalRadius) / (deltaLength * (p1.invmass + p2.invmass));

        i.point    = -p1.invmass*delta*diff;
        i.responseGradient =  p2.invmass*delta*diff;
        return true;
    }
    return false;
}
