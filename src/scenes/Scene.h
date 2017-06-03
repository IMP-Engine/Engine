#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <dirent.h>
#ifndef __gl_h_
#include "../glad/glad.h"
#endif
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "../imgui/imgui.h"

#include "../glHelper.h"
#include "../primitives.h"

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

    std::vector<glm::vec3> loadedVertices;
    std::vector<glm::vec3> normalVectors;
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<int> indices;
    std::vector<int> normalIndices;
    std::vector<Triangle> triangles;
    int numTriangles;

    void loadSceneNames();
    void loadScene(std::string scene);
    void gui(bool *show);
    void render(glm::mat4 &viewMatrix, glm::mat4 &projectionMatrix, const glm::vec3 &lightPosition);
    void init();
};

