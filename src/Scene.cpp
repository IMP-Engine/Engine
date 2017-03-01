#include "Scene.h"



Scene::Scene()
{
    // Cube setup
    unsigned short cubeIndices[] = {
        // front
        2, 1, 0,
        2, 0, 3,
        // top
        2, 5, 1,
        2, 6, 5,
        // back
        4, 5, 6,
        4, 6, 7,
        // bottom
        4, 7, 3,
        4, 3, 0,
        // left
        4, 0, 1,
        4, 1, 5,
        // right
        2, 3, 7,
        2, 7, 6
    };

    float cubeVertices[] = {
        -10.0f, -10.0f,  10.0f,
        10.0f, -10.0f,  10.0f,
        10.0f,  10.0f,  10.0f,
        -10.0f,  10.0f,  10.0f,

        -10.0f, -10.0f, -10.0f,
        10.0f, -10.0f, -10.0f,
        10.0f,  10.0f, -10.0f,
        -10.0f,  10.0f, -10.0f,
    };

    float normalComponents[] = {
        -0.5773f, -0.5773f,  0.5773f,
         0.5773f, -0.5773f,  0.5773f,
         0.5773f,  0.5773f,  0.5773f,
        -0.5773f,  0.5773f,  0.5773f,

        -0.5773f, -0.5773f, -0.5773f,
         0.5773f, -0.5773f, -0.5773f,
         0.5773f,  0.5773f, -0.5773f,
        -0.5773f,  0.5773f, -0.5773f,
    };
    for (int i = 0; i < 24; i++)
    {
        vertexes.push_back(cubeVertices[i]);
    }
    for (int i = 0; i < 36; i++)
    {
        indices.push_back(cubeIndices[i]);
    }
    for (int i = 0; i < 24; i++)
    {
        normals.push_back(normalComponents[i]);
    }


}


Scene::~Scene()
{
    delete &vertexes;
    delete &indices;
    delete &normals;
}
