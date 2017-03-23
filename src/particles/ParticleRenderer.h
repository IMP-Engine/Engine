#pragma once

#include <vector>
#include "../glad/glad.h"
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
    unsigned int *particleCount;

	void init(ParticleData particles);
	void render(glm::mat4 &modelViewProjectionMatrix, glm::mat4 &modelViewMatrix, glm::vec3 &viewSpaceLightPosition, glm::mat4 &projectionMatrix);
};

