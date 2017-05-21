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

void ModelData::addVertices(std::vector<int> &e, std::vector<glm::vec3> &bc, std::vector<glm::ivec3> &particles, ModelData &data)
{
    for (unsigned int i = 0; i < bc.size(); ++i)
    {
        for (unsigned short j = 0; j < 3; j++)
        {
            data.bcCoords.push_back(bc[i][j]);
            data.closestParticles.push_back(particles[i][j]);
        }
    }

    data.elements.insert(std::end(data.elements), std::begin(e), std::end(e));
}
