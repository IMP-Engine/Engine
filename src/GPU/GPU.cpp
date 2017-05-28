#include "gpu.h"

GPU::GPU()
{
    glGenFramebuffers(1, &fbo);
    program = glHelper::loadShader(VERTEX_SHADER_PATH, SC_SHADER_PATH);

    glGenBuffers(1, &vbo);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
        glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    glGenTextures(1, &posMain);
    glGenTextures(1, &posSub);
    glGenTextures(1, &invmasses);
    glGenTextures(1, &constraint);
}

GPU::~GPU()
{
    glDeleteFramebuffers(1, &fbo);
    glDeleteProgram(program);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteTextures(1, &posMain);
    glDeleteTextures(1, &posSub);
    glDeleteTextures(1, &invmasses);
    glDeleteTextures(1, &constraint);
}

void initiateTexture(GLuint width, GLuint height, GLint internalFormat, GLenum format, void* data) {

    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexImage2D(target, 0, internalFormat, width, height, 0, format, GL_FLOAT, data);
}

void GPU::restart(ParticleData & particles, ConstraintData & constraints)
{
    pWidth = particles.cardinality; pHeight = 1; cWidth = constraints.distanceConstraints.cardinality; cHeight = 1;
    // Particle data
    glBindTexture(target, posMain);
    initiateTexture(pWidth, pHeight, GL_RGB32F, GL_RGB, &particles.pPosition[0]);
    glBindTexture(target, posSub);
    initiateTexture(pWidth, pHeight, GL_RGB32F, GL_RGB, NULL);
    glBindTexture(target, invmasses);
    initiateTexture(pWidth, pHeight, GL_R32F, GL_RED, &particles.invmass[0]);

    glBindTexture(target, constraint);
    float* data = (float*)malloc(cWidth * cHeight * sizeof(glm::vec4));
    for (int i = 0; i < cWidth * cHeight; i += 4)
    {
        data[i] = (float) constraints.distanceConstraints.particles[i].x;     // eps ?
        data[i + 1] = (float) constraints.distanceConstraints.particles[i].y; // eps ?
        data[i + 2] = constraints.distanceConstraints.distance[i];
        data[i + 3] = constraints.distanceConstraints.stiffness[i];
    }
    initiateTexture(cWidth, cHeight, GL_RGBA32F, GL_RGBA, data);
    free(data);
}

void GPU::run(ParticleData & particles, ConstraintData & constraints)
{
    // There should not be any need to upload data every iteration
    this->restart(particles, constraints);
    
    glUseProgram(program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(target, posMain);
    glUniform1i(glGetUniformLocation(program, "oldPos"), 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(target, posMain);
    glUniform1i(glGetUniformLocation(program, "invmasses"), 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(target, posMain);
    glUniform1i(glGetUniformLocation(program, "constraints"), 2);
    
    glViewport(0, 0, pWidth, pHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, posSub, 0);
    
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    
    // There should not be any need to actually download data each iteration
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(0, 0, pWidth, pHeight, GL_RGB, GL_FLOAT, &particles.pPosition[0]);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);
    std::swap(posMain, posSub);

}
