#pragma once
#include "Constraint.h"
#include "Particle.h"
#include <glm/geometric.hpp>
#include <glm/gtx/norm.hpp>

using namespace glm;

class BendingConstraint :
    public Constraint
{
public:
    float angle;

    BendingConstraint(Particle* p1, Particle* p2, Particle* p3, Particle* p4, float stiffness, float angle, bool equality = true) {
        this->particles.push_back(p1);
        this->particles.push_back(p2);
        this->particles.push_back(p3);
        this->particles.push_back(p4);
        this->stiffness = stiffness;
        this->angle = angle;
        this->equality = equality;
    }

    bool evaluate() {
            return equality || (acos(calcDotNormals()) - this->angle) < 0;
    }
    
    float evaluateScaleFactor() {
        float accum = (acos(calcDotNormals()) - this->angle) * sqrt(1- pow(calcDotNormals(), 2.0f));

        for (std::vector<Particle*>::iterator it = this->particles.begin(); it != this->particles.end(); it++)
        {
            accum += (*it)->invmass + length2(evaluateGradient(it));
        }
        
        return accum;
    }

    vec3 evaluateGradient(std::vector<Particle*>::iterator p) {
        float d = calcDotNormals();
        const vec3 p1 = this->particles[0]->pos;
        const vec3 p2 = this->particles[1]->pos;
        const vec3 p3 = this->particles[2]->pos;
        const vec3 p4 = this->particles[3]->pos;
        const vec3 n1 = cross(p2 - p1, p3 - p1);
        const vec3 n2 = cross(p2 - p1, p4 - p1);

        switch (p - this->particles.begin())
        {
        case 0:    return -evaluateGradient(p+1) - evaluateGradient(p + 2) - evaluateGradient(p + 3);
        case 1:    return -(cross(p3, n2) + d*(cross(n1, p3))) / length(cross(p2, p3)) - (cross(p4, n1) + d*(cross(n2, p4))) / length(cross(p2, p4));
        case 2:    return (cross(p2, n2) + d*(cross(n1, p2))) / length(cross(p2, p3));
        case 3:    return (cross(p2, n1) + d*(cross(n2, p2))) / length(cross(p2, p4));
        }
    }

private:
    // Dot product of the normals of two contiguous triangle sections
    float calcDotNormals() {
        const vec3 p1 = this->particles[0]->pos;
        const vec3 p2 = this->particles[1]->pos;
        const vec3 p3 = this->particles[2]->pos;
        const vec3 p4 = this->particles[3]->pos;
        const vec3 n1 = cross(p2 - p1, p3 - p1);
        const vec3 n2 = cross(p2 - p1, p4 - p1);
        
        return dot(normalize(n1), normalize(n2));
    }
};

