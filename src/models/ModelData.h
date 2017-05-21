#pragma once
#include <vector>
#ifndef __gl_h_
#include "../glad/glad.h"
#endif
#include "glm/glm.hpp"

#define MAX_VERTICES 200000

class ModelData
{
public:
    ModelData();
    ~ModelData();

    std::vector<int> elements;
    std::vector<float> bcCoords;
    std::vector<int> closestParticles;

    void clear();

    void addVertices(std::vector<int> &e, std::vector<glm::vec3> &bc, std::vector<glm::ivec3> &particles, ModelData &data);
};
