#pragma once

#include <vector>
#include "../glad/glad.h"
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "../glHelper.h"
#include "Particle.h"




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
	ParticleRenderer(std::vector<Particle> *particleVector) : particles(particleVector) { };
	~ParticleRenderer();
	std::vector<Particle> *particles;

	GLuint particleShader;
	GLuint vao;
	GLuint particleBuffer;

	void init();
	void render(mat4 &modelViewProjectionMatrix, mat4 &modelViewMatrix, vec3 &viewSpaceLightPosition, mat4 &projectionMatrix);
};

