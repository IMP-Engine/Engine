#pragma once

#include <vector>


class Scene
{
public:
    Scene();
    ~Scene();
    unsigned int vbo;
    unsigned int vao;
    unsigned int ibo;
    unsigned int normalBuffer;
    std::vector<float> vertexes;
    std::vector<unsigned short> indices;
    std::vector<float> normals;
    int numTriangles;
};

