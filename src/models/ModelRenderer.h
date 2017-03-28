#pragma once

#include <vector>
#include "../glad/glad.h"
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "../glHelper.h"
#include "ModelData.h"

#ifdef _WIN32
#define VERT_SHADER_PATH "../../src/shaders/simple.vert.glsl"
#define FRAG_SHADER_PATH "../../src/shaders/simple.frag.glsl"
#elif __unix__
#define VERT_SHADER_PATH "../src/shaders/simple.vert.glsl"
#define FRAG_SHADER_PATH "../src/shaders/simple.frag.glsl"
#endif

class ModelRenderer
{
public:
	ModelRenderer();
	~ModelRenderer();

	GLuint simpleShader;
	GLuint vao;
	GLuint vbo;
    GLuint ibo;
    unsigned int *particleCount;

	void init();
	void render(ModelData &data, glm::mat4 &modelViewProjectionMatrix, glm::mat4 &modelViewMatrix, glm::vec3 &viewSpaceLightPosition, glm::mat4 &projectionMatrix);
};

