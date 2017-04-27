#pragma once

#include <vector>
#ifndef __gl_h_
#include "../glad/glad.h"
#endif // !__gl_h_

#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "../glHelper.h"
#include "ModelData.h"
#include "../particles//ParticleData.h"

#ifdef _WIN32
#define SURFACE_VERT_SHADER_PATH "../../src/shaders/simple.vert"
#define SURFACE_FRAG_SHADER_PATH "../../src/shaders/simple.frag"
#elif __unix__
#define SURFACE_VERT_SHADER_PATH "../src/shaders/simple.vert"
#define SURFACE_FRAG_SHADER_PATH "../src/shaders/simple.frag"
#endif

class ModelRenderer
{
public:
	ModelRenderer();
	~ModelRenderer();

	GLuint simpleShader;
	GLuint vao;
	GLuint vbo;
    GLuint nbo;
    GLuint ibo;
    unsigned int *particleCount;

	void init();
	void render(ParticleData &particles, ModelData &data, glm::mat4 &modelViewProjectionMatrix, glm::mat4 &modelViewMatrix, const glm::vec3 &lightPosition, glm::mat4 &projectionMatrix);
};

