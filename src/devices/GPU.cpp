#include "gpu.h"

GPU::GPU(ParticleData & particles, ConstraintData & constraints, std::vector<Triangle>& triangles)
    : Device(particles, constraints, triangles)
{
    glGenFramebuffers(1, &fbo);

    constraintSolveProgram = glHelper::loadShader(VERTEX_SHADER_PATH, SC_SHADER_PATH);
    verletIntegrationProgram = glHelper::loadShader(VERTEX_SHADER_PATH, VI_SHADER_PATH);
    updateVelocityProgram = glHelper::loadShader(VERTEX_SHADER_PATH, UV_SHADER_PATH);
    updatePositionProgram = glHelper::loadShader(VERTEX_SHADER_PATH, UP_SHADER_PATH);

    glGenBuffers(1, &vbo);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    glGenTextures(1, &bufMain);
    glGenTextures(1, &bufSub);
    glGenTextures(1, &posStart);
    glGenTextures(1, &invmasses);
    glGenTextures(1, &boundConstraints); // TODO remove in favor of GS
    glGenTextures(1, &constraint);
    glGenTextures(1, &segment);
    glGenTextures(1, &indirect);
}

GPU::~GPU()
{
    glDeleteFramebuffers(1, &fbo);
    glDeleteProgram(constraintSolveProgram);
    glDeleteProgram(constraintSolveProgram);
    glDeleteProgram(updateVelocityProgram);
    glDeleteProgram(updatePositionProgram);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteTextures(1, &bufMain);
    glDeleteTextures(1, &bufSub);
    glDeleteTextures(1, &posStart);
    glDeleteTextures(1, &invmasses);
    glDeleteTextures(1, &boundConstraints); // TODO remove in favor of GS
    glDeleteTextures(1, &constraint);
    glDeleteTextures(1, &segment);
    glDeleteTextures(1, &indirect);
}

void initiateTexture(GLuint width, GLuint height, GLint internalFormat, GLenum format, GLenum type, void* data) {

    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(target, 0, internalFormat, width, height, 0, format, type, data);
}

void GPU::start(ParticleData & particles, ConstraintData & constraints)
{
    // TODO is it better to store texture with height one or try to make them as square as possible?
    pWidth = particles.cardinality; pHeight = 1; cWidth = constraints.distanceConstraints.cardinality; cHeight = 1;
    // Particle data
    glBindTexture(target, bufMain);
    initiateTexture(pWidth, pHeight, GL_RGB32F, GL_RGB, GL_FLOAT, NULL);
    glBindTexture(target, bufSub);
    initiateTexture(pWidth, pHeight, GL_RGB32F, GL_RGB, GL_FLOAT, NULL);
    glBindTexture(target, posStart);
    initiateTexture(pWidth, pHeight, GL_RGB32F, GL_RGB, GL_FLOAT, &particles.position);
    glBindTexture(target, invmasses);
    initiateTexture(pWidth, pHeight, GL_R32F, GL_RED, GL_FLOAT, &particles.invmass[0]);
    // TODO remove in favor of GS
    glBindTexture(target, boundConstraints);
    initiateTexture(pWidth, pHeight, GL_R32I, GL_RED_INTEGER, GL_INT, &particles.numBoundConstraints[0]);

    glBindTexture(target, constraint);
    glm::vec4* data = (glm::vec4*)malloc(cWidth * cHeight * sizeof(glm::vec4));
    for (int i = 0; i < cWidth * cHeight; i++)
    {
        data[i] = glm::vec4(
            (float) constraints.distanceConstraints.particles[i].x, // eps ?
            (float) constraints.distanceConstraints.particles[i].y, // eps ?
            constraints.distanceConstraints.distance[i],
            constraints.distanceConstraints.stiffness[i]);
    }
    initiateTexture(cWidth, cHeight, GL_RGBA32F, GL_RGBA, GL_FLOAT, data);
    free(data);

    // This is where things start to get a little bit weird
    std::vector<GLint> indirectionList;
    std::vector<GLint> segmentList;
    std::vector<GLint> tempList;

    indirectionList.push_back(0);
    for (GLint i = 0; i < particles.cardinality; i++)
    {
        for (GLint j = 0; j < constraints.distanceConstraints.cardinality; j++)
        {
            if (constraints.distanceConstraints.particles[j].x == i ||
                constraints.distanceConstraints.particles[j].y == i) {
                tempList.push_back(j);
            }
        }
        segmentList.insert(segmentList.end(), tempList.begin(), tempList.end());
        indirectionList.push_back(segmentList.size());
        tempList.clear();
    }
    glBindTexture(target, segment);
    initiateTexture(segmentList.size(), 1, GL_R32I, GL_RED_INTEGER, GL_INT, &segmentList[0]);
    glBindTexture(target, indirect);
    initiateTexture(indirectionList.size(), 1, GL_R32I, GL_RED_INTEGER, GL_INT, &indirectionList[0]);
    glBindTexture(target, 0);
}

void GPU::stop(ParticleData & particles)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, posStart, 0);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(0, 0, pWidth, pHeight, GL_RGB, GL_FLOAT, &particles.position[0]);
}

void GPU::verletIntegrate(float dt, glm::vec3 forces)
{
    glUseProgram(verletIntegrationProgram);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(target, posStart);
    glUniform1i(glGetUniformLocation(verletIntegrationProgram, "posStart"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(target, bufMain);
    glUniform1i(glGetUniformLocation(verletIntegrationProgram, "velocity"), 1);

    glUniform1f(glGetUniformLocation(verletIntegrationProgram, "dt"), dt);
    glUniform3fv(glGetUniformLocation(verletIntegrationProgram, "forces"), 1, &forces[0]);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, bufSub, 0);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);
    std::swap(bufMain, bufSub);
}

void GPU::solveConstraintsJacobi(int iterations, float omega)
{   
    glUseProgram(constraintSolveProgram);
    
    // TODO no need to find position every frame
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(target, bufMain);
    glUniform1i(glGetUniformLocation(constraintSolveProgram, "oldPos"), 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(target, invmasses);
    glUniform1i(glGetUniformLocation(constraintSolveProgram, "invmasses"), 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(target, constraint);
    glUniform1i(glGetUniformLocation(constraintSolveProgram, "constraints"), 2);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(target, segment);
    glUniform1i(glGetUniformLocation(constraintSolveProgram, "segments"), 3);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(target, indirect);
    glUniform1i(glGetUniformLocation(constraintSolveProgram, "indirections"), 4);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(target, boundConstraints);
    glUniform1i(glGetUniformLocation(constraintSolveProgram, "boundConstraints"), 5);

    glUniform1f(glGetUniformLocation(constraintSolveProgram, "omega"), omega);
    
    glViewport(0, 0, pWidth, pHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindVertexArray(vao);

    for (int i = 1; i <= iterations; i++) {
        glUniform1i(glGetUniformLocation(constraintSolveProgram, "iteration"), i);
        // TODO Swap by rendering to different color attachment https://www.seas.upenn.edu/~cis565/fbo.htm#feedback2 and test what is more speed
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, bufSub, 0);
    
        glDrawArrays(GL_TRIANGLES, 0, 6);
        std::swap(bufMain, bufSub);
    }

    glBindVertexArray(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);
    
}

void GPU::updateParticles(float dt, float pSleeping)
{
    glViewport(0, 0, pWidth, pHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindVertexArray(vao);

    glUseProgram(updatePositionProgram);
    // TODO no need to find position every frame
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(target, bufMain);
    glUniform1i(glGetUniformLocation(updatePositionProgram, "pos"), 0);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, posStart, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glUseProgram(updateVelocityProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(target, posStart);
    glUniform1i(glGetUniformLocation(constraintSolveProgram, "oldPos"), 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(target, bufMain);
    glUniform1i(glGetUniformLocation(constraintSolveProgram, "pos"), 1);

    glUniform1f(glGetUniformLocation(constraintSolveProgram, "dt"), dt);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, bufSub, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);
}

void GPU::solveConstraintsGaussSeidel(int iterations, float omega)
{
    throw std::exception("Not implemented");
}

void GPU::detectCollisionsTriangles()
{
}

void GPU::detectCollisionParticles()
{
}

void GPU::stabilizationPass(int stabilizationIterations)
{
}

void GPU::dampCollision(float restitutionCoefficientT, float restitutionCoefficientN)
{
}

void GPU::renderParticles(glm::mat4 & modelViewProjectionMatrix, glm::mat4 & modelViewMatrix, glm::vec3 & viewSpaceLightPosition, glm::mat4 & projectionMatrix, int height)
{
}
