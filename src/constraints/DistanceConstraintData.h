#pragma once

#include <vector>
#include "glm/glm.hpp"

#include "../../tbb/include/tbb/mutex.h"
#include "../particles/ParticleData.h"
#include "../tbb/include/tbb/parallel_for.h"

#ifndef MAX_DISTANCE_CONSTRAINTS
#define MAX_DISTANCE_CONSTRAINTS 200000
#endif // !MAX_DISTANCE_CONSTRAINTS


class DistanceConstraintData
{
public:
    DistanceConstraintData();
    ~DistanceConstraintData();

    int cardinality;

    std::vector<ivec2> particles; // Assume there is 2 particles 
    std::vector<float> stiffness;
    std::vector<float> distance;
    std::vector<bool>  equality;
    std::vector<float> threshold;

    float evaluate(int constraintIndex, ParticleData &particleData);

    bool solveDistanceConstraint(glm::vec3 &delta1, glm::vec3 &delta2, int constraintIndex, ParticleData &particleData);

    void clear();
    void removeBroken(ParticleData &particleData);

    /*
        Used for parallelizing distance constraint solving
    */
    class SolveDistanceConstraint {
    public:
        ParticleData & particles;
        DistanceConstraintData & distanceConstraints;
        float overRelaxConst;
        int iterationNum;

        void operator()(const tbb::blocked_range<size_t>& r) const {
            vec3 delta1(0);
            vec3 delta2(0);
            std::vector<int> &numBoundConstraints = particles.numBoundConstraints;
            std::vector<vec3> &pPosition = particles.pPosition;
            for (size_t constraintIndex = r.begin(); constraintIndex != r.end(); ++constraintIndex)
            {
                if (distanceConstraints.solveDistanceConstraint(delta1, delta2, constraintIndex, particles))
                {
                    ivec2 &constraintParticles = distanceConstraints.particles[constraintIndex];
                    int p1 = constraintParticles[0];
                    int p2 = constraintParticles[1];
                    pPosition[p1] -=
                        delta1
                        * (1 - pow(1 - distanceConstraints.stiffness[constraintIndex], 1 / (float)iterationNum))
                        * overRelaxConst
                        / (float)numBoundConstraints[p1];

                    pPosition[p2] -=
                        delta2
                        * (1 - pow(1 - distanceConstraints.stiffness[constraintIndex], 1 / (float)iterationNum))
                        * overRelaxConst
                        / (float)numBoundConstraints[p2];
                }
            }
        }
        SolveDistanceConstraint(ParticleData & a, DistanceConstraintData & b, float c, int d) :
            particles(a), distanceConstraints(b), overRelaxConst(c), iterationNum(d)
        {}
    };

};
struct DistanceConstraint {
    int firstParticleIndex;
    int secondParticleIndex;
    float stiffness;
    float distance;
    float threshold;
    bool equality;
};


void addConstraint(DistanceConstraintData &data, DistanceConstraint &config);

