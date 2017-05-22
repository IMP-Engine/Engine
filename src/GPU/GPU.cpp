#include "gpu.h"
#include <fstream>

int convertToString(const char *filename, std::string& s)
{
    size_t size;
    char*  str;
    std::fstream f(filename, (std::fstream::in | std::fstream::binary));

    if (f.is_open())
    {
        size_t fileSize;
        f.seekg(0, std::fstream::end);
        size = fileSize = (size_t)f.tellg();
        f.seekg(0, std::fstream::beg);
        str = new char[size + 1];
        if (!str)
        {
            f.close();
            return 0;
        }

        f.read(str, fileSize);
        f.close();
        str[size] = '\0';
        s = str;
        delete[] str;
        return 0;
    }
    std::cout << "Error: failed to open file\n:" << filename << std::endl;
    return 0;
}


GPU::GPU()
{
    cl_int status;
    cl_uint numPlatforms;
    cl_platform_id platform = NULL;

    /* Platform */

    status = clGetPlatformIDs(0, NULL, &numPlatforms);
    CHECK_CL_ERROR(status, "Failed to get platform");

    if (numPlatforms > 0)
    {
        cl_platform_id* platforms = (cl_platform_id*)malloc(numPlatforms * sizeof(cl_platform_id));
        status = clGetPlatformIDs(numPlatforms, platforms, NULL);
        platform = platforms[0];
        free(platforms);
    }

    /* Device */

    cl_uint numDevices = 0;
    cl_device_id *devices;
    status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL, &numDevices);

    if (numDevices == 0)
    {
        std::cerr << "No GPU found. Exiting..."; // USE CPU
        exit(EXIT_FAILURE);
        /*
        status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 0, NULL, &numDevices);
        devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));
        status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, numDevices, devices, NULL);
        */
    }
    else
    {
        devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));
        clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, numDevices, devices, NULL);
    }

    /* Context */

    context = clCreateContext(NULL, 1, devices, NULL, NULL, NULL);

    /* Command queue */

    commandQueue = clCreateCommandQueue(context, devices[0], 0, &status);
    CHECK_CL_ERROR(status, "Failed to create command queue");

    /* Create program */
    const char* filename = "../../src/HelloWorld_Kernel.cl";
    std::string sourceStr;
    status = convertToString(filename, sourceStr);
    const char *source = sourceStr.c_str();
    size_t sourceSize[] = { strlen(source) };
    program = clCreateProgramWithSource(context, 1, &source, sourceSize, NULL);

    /* Build program */
    status = clBuildProgram(program, 1, devices, NULL, NULL, NULL);
    if (status == CL_BUILD_PROGRAM_FAILURE) {
        // Determine the size of the log
        size_t log_size;
        clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

        // Allocate memory for the log
        char *log = (char *)malloc(log_size);

        // Get the log
        clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, log_size, log, NULL);

        // Print the log
        printf("%s\n", log);
    }
    CHECK_CL_ERROR(status, "Failed to build program");

    /* Kernel object */
    kernel = clCreateKernel(program, "helloworld", &status);
    CHECK_CL_ERROR(status, "Failed to create kernel");


    if (devices != NULL)
    {
        free(devices);
        devices = NULL;
    }
}

GPU::~GPU()
{
    cl_int status;
    status = clReleaseKernel(kernel);
    CHECK_CL_ERROR(status, "Failed to release kernel");
    status = clReleaseProgram(program);	
    CHECK_CL_ERROR(status, "Failed to release program");
    status = clReleaseCommandQueue(commandQueue);	
    CHECK_CL_ERROR(status, "Failed to release command queue");
    status = clReleaseContext(context);				
    CHECK_CL_ERROR(status, "Failed to release context");

}

void GPU::run(ParticleData & particles, ConstraintData & constraints)
{
    cl_int status;

    /* memory objects */
    cl_mem pPosBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, particles.cardinality * sizeof(glm::vec3), (void*)&particles.pPosition[0].x, NULL);

    cl_mem invmassBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, particles.cardinality * sizeof(float), (void*)&particles.invmass[0], NULL);

    cl_mem boundConstraintsBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, particles.cardinality * sizeof(int), (void*)&particles.numBoundConstraints[0], NULL);

    cl_mem particlesBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, constraints.distanceConstraints.cardinality * sizeof(glm::vec2), (void*)&constraints.distanceConstraints.particles[0].x, NULL);

    cl_mem distancesBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, constraints.distanceConstraints.cardinality * sizeof(float), (void*)&constraints.distanceConstraints.distance[0], NULL);

    cl_mem equalitiesBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, constraints.distanceConstraints.cardinality * sizeof(bool), (void*)&constraints.distanceConstraints.equality[0], NULL);

    std::vector<glm::vec3> deltas;
    deltas.resize(particles.cardinality, glm::vec3(0));

    cl_mem deltasBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, particles.cardinality * sizeof(glm::vec3), (void*)&deltas[0].x, NULL);

    /* Set kernel arguments */
    status = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&pPosBuffer);
    CHECK_CL_ERROR(status, "Failed to set pPosBuffer");
    status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&invmassBuffer);
    CHECK_CL_ERROR(status, "Failed to set invmassBuffer");
    status = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&boundConstraintsBuffer);
    CHECK_CL_ERROR(status, "Failed to set boundConstraintsBuffer");
    status = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*)&particlesBuffer);
    CHECK_CL_ERROR(status, "Failed to set particlesBuffer");
    status = clSetKernelArg(kernel, 4, sizeof(cl_mem), (void*)&distancesBuffer);
    CHECK_CL_ERROR(status, "Failed to set distancesBuffer");
    status = clSetKernelArg(kernel, 5, sizeof(cl_mem), (void*)&equalitiesBuffer);
    CHECK_CL_ERROR(status, "Failed to set equalitiesBuffer");
    status = clSetKernelArg(kernel, 6, sizeof(cl_mem), (void*)&deltasBuffer);
    CHECK_CL_ERROR(status, "Failed to set deltasBuffer");

    /* Run kernel */
    size_t global_work_size[1] = { particles.cardinality };
    status = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, global_work_size, NULL, 0, NULL, NULL);

    /* Read output into host memory */
    status = clEnqueueReadBuffer(commandQueue, deltasBuffer, CL_TRUE, 0, particles.cardinality * sizeof(glm::vec3), (void*)&deltas[0].x, 0, NULL, NULL);

    


    /*status = clReleaseMemObject(inputBuffer);
    CHECK_CL_ERROR(status, "Failed to release inputbuffer");
    status = clReleaseMemObject(outputBuffer);
    CHECK_CL_ERROR(status, "Failed to release outputbuffer");*/
}
