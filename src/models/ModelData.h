#pragma once
#include <vector>
#include "../glad/glad.h"

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

    void addVertices(std::vector<int> &e, std::vector<float[3]> &bc, std::vector<int[3]> &particles, ModelData &data);
};
