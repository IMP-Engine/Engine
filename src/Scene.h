#pragma once

#include <vector>

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glHelper.h"
#include "primitives.h"

#ifdef _WIN32
#define VERT_SHADER_PATH "../../src/shaders/simple.vert"
#define FRAG_SHADER_PATH "../../src/shaders/simple.frag"
#elif __unix__
#define VERT_SHADER_PATH "../src/shaders/simple.vert"
#define FRAG_SHADER_PATH "../src/shaders/simple.frag"
#endif


class Scene
{
public:
    Scene();
    unsigned int vbo;
    unsigned int vao;
    unsigned int ibo;
    unsigned int normalBuffer;
    unsigned int shader;

    glm::mat4 modelMatrix;

    std::vector<float> vertexes;
    std::vector<unsigned short> indices;
    std::vector<float> normals;
    std::vector<Triangle> triangles;
    int numTriangles;

    void render(glm::mat4 &viewMatrix, glm::mat4 &projectionMatrix, const glm::vec3 &lightPosition);
    void init();
};

