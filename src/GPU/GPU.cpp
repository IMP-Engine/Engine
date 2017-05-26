#include "gpu.h"

GPU::GPU()
{
    glGenFramebuffers(1, &fbo);

    glGenTextures(1, &posMain);
    glGenTextures(1, &posSub);
    glGenTextures(1, &invmasses);
    glGenTextures(1, &constraint);
}

GPU::~GPU()
{
    glDeleteFramebuffers(1, &fbo);
    glDeleteProgram(program);
    glDeleteTextures(1, &posMain);
    glDeleteTextures(1, &posSub);
    glDeleteTextures(1, &invmasses);
    glDeleteTextures(1, &constraint);
}

void initiateTexture(float width, float height, GLint internalFormat, GLenum format, void* data) {

    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexImage2D(target, 0, internalFormat, width, height, 0, format, GL_FLOAT, data);
}

void GPU::restart(ParticleData & particles, ConstraintData & constraints)
{
    // Particle data
    glBindTexture(target, posMain);
    initiateTexture(10, 10, GL_RGB32F, GL_RGB, &particles.pPosition[0]);
    glBindTexture(target, posSub);
    initiateTexture(10, 10, GL_RGB32F, GL_RGB, NULL);
    glBindTexture(target, invmasses);
    initiateTexture(10, 10, GL_R32F, GL_RED, &particles.invmass[0]);

    glBindBuffer(target, constraint);
    float* data = (float*)malloc(constraints.distanceConstraints.cardinality * sizeof(glm::vec4));
    for (int i = 0; i < constraints.distanceConstraints.cardinality; i += 4)
    {
        data[i] = (float) constraints.distanceConstraints.particles[i].x;     // eps ?
        data[i + 1] = (float) constraints.distanceConstraints.particles[i].y; // eps ?
        data[i + 2] = constraints.distanceConstraints.distance[i];
        data[i + 3] = constraints.distanceConstraints.stiffness[i];
    }
    initiateTexture(100, 100, GL_RGBA32F, GL_RGBA, data);
    free(data);
}

void GPU::run(ParticleData & particles, ConstraintData & constraints)
{
    //DEBUG
    float* res = (float*)malloc(10 * 10 * sizeof(glm::vec3));
    memset(res, 1, 10 * 10 * sizeof(glm::vec3));
    
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, posMain, 0);

    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(0, 0, 10, 10, GL_RGB, GL_FLOAT, res);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    std::swap(posMain, posSub);

    // DEBUG
    free(res);
}
