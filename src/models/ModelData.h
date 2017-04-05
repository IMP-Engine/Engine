#pragma once
#include <vector>
#include "../glad/glad.h"

#define MAX_VERTICES 200000

class ModelData
{
public:
	ModelData();
	~ModelData();

    std::vector<GLushort> elements;
    std::vector<float> bcCoords;
    std::vector<int> closestParticles;

    void clear();

    void addVertices(std::vector<short> &e, std::vector<float[3]> &bc, std::vector<int[3]> &particles, ModelData &data);
};
