#pragma once
#include <vector>

class ModelData
{
public:
	ModelData();
	~ModelData();
    unsigned int cardinality;

    std::vector<short> elements;
    std::vector<float> bcCoords;
    std::vector<int> closestParticles;

    void clear();
};

void addVertices(std::vector<short> &e, std::vector<float[3]> &bc, std::vector<int[3]> &particles,  ModelData &data);
