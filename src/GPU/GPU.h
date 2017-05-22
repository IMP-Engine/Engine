#pragma once
#include <CL\opencl.h>
#include <iostream>
#include <string>
#include <glm\common.hpp>

#include "../particles/ParticleData.h"
#include "../constraints/ConstraintData.h"

#define CHECK_CL_ERROR( status, msg ) \
    if (status != CL_SUCCESS) \
    { \
        std::cout << "Error: " << msg << std::endl; \
        std::cout << "At " << __FILE__ << ":" << __LINE__ << std::endl; \
        std::cout << "With errorcode " << status << std::endl; \
        exit(EXIT_FAILURE); \
    } \

class GPU
{
public:
    
    GPU();
    ~GPU();

    void run(ParticleData & particles, ConstraintData & constraints);

private:
   
    cl_kernel kernel;
    cl_program program;
    cl_command_queue commandQueue;
    cl_context context;

};