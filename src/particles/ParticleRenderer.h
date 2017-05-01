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
#define PARTICLE_FRAG_SHADER_PATH "../../src/shaders/particle.frag.glsl"
#elif __unix__
#define PARTICLE_VERT_SHADER_PATH "../src/shaders/particle.vert.glsl"
#define PARTICLE_FRAG_SHADER_PATH "../src/shaders/particle.frag.glsl"
#endif

class ParticleRenderer
{
public:
    ParticleRenderer();
    ~ParticleRenderer();

    GLuint particleShader;
    GLuint vao;
    GLuint particleBuffer;
    GLuint sizeBuffer;
    unsigned int *particleCount;

    void init();
    void render(ParticleData &particles, glm::mat4 &modelViewProjectionMatrix, glm::mat4 &modelViewMatrix, glm::vec3 &viewSpaceLightPosition, glm::mat4 &projectionMatrix, GLint height);
};

