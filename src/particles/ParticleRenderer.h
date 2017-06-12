#pragma once

#include <vector>
#ifndef __gl_h_
#include "../glad/glad.h"
#endif // !__gl_h_

#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "../glHelper.h"
#include "ParticleData.h"

#ifdef _WIN32
#define PARTICLE_VERT_SHADER_PATH "../../src/shaders/particle.vert.glsl"
#define PARTICLE_VERT_GPU_SHADER_PATH "../../src/shaders/particleGPU.vert.glsl"
#define PARTICLE_FRAG_SHADER_PATH "../../src/shaders/particle.frag.glsl"
#elif __unix__
#define PARTICLE_VERT_SHADER_PATH "../src/shaders/particle.vert.glsl"
#define PARTICLE_VERT_GPU_SHADER_PATH "../src/shaders/particleGPU.vert.glsl"
#define PARTICLE_FRAG_SHADER_PATH "../src/shaders/particle.frag.glsl"
#endif

class ParticleRenderer
{
public:
    ParticleRenderer();
    ~ParticleRenderer();

    GLuint particleShader;
    GLuint particleShaderGPU;
    GLuint vao;
    GLuint particleBuffer;
    GLuint sizeBuffer;
    GLuint phaseBuffer;
    unsigned int *particleCount;

    void init();
    void render(ParticleData &particles, glm::mat4 &modelViewProjectionMatrix, glm::mat4 &modelViewMatrix, glm::vec3 &viewSpaceLightPosition, glm::mat4 &projectionMatrix, GLint height);
    void render(int numParticles, GLuint particles, GLuint radii, glm::mat4 &modelViewProjectionMatrix, glm::mat4 &modelViewMatrix, glm::vec3 &viewSpaceLightPosition, glm::mat4 &projectionMatrix, GLint height);
};

