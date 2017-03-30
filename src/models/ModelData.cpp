#include "ModelData.h"

ModelData::ModelData()
{
    elements.reserve(MAX_VERTICES);
    bcCoords.reserve(MAX_VERTICES);
    closestParticles.reserve(MAX_VERTICES);
}

ModelData::~ModelData()
{
}

void ModelData::clear()
{
    elements.clear();
    bcCoords.clear();
    closestParticles.clear();
}

void ModelData::addVertices(std::vector<short> &e, std::vector<float[3]> &bc, std::vector<int[3]> &particles, ModelData &data)
{
    for (int i = 0; i < bc.size(); i++)
    {
        for (int j = 0; j < 3; j++)
        {
            data.elements.push_back(e[(3 * i) + j]);
            data.bcCoords.push_back(bc[i][j]);
            data.closestParticles.push_back(particles[i][j]);
        }
    }
}
