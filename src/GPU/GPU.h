#pragma once

#include "../particles/ParticleData.h"
#include "../constraints/ConstraintData.h"



class GPU
{
public:
    
    GPU();
    ~GPU();

    void run(ParticleData & particles, ConstraintData & constraints);

private:
   
};