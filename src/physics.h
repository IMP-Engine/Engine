#pragma once

#include <vector>
#include <stdio.h>
#include <algorithm>
#include <stdio.h>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/norm.hpp>

#include "particles/ParticleData.h"
#include "Scene.h"
#include "constraints/Constraint.h"
#include "intersections.h"
#include "performance.h"


class Physics 
{
public:
    Physics();
    ~Physics();

    float overRelaxConst;
    float pSleeping;
    float restitutionCoefficient;

    ParticleData particles;
    std::vector<Constraint*> constraints;

	void step(Scene *scene, float dt, int iterations);
};
