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

    for (unsigned int i = 0; i < indices.size();i += 3) {
        Triangle t = Triangle();
        t.v0 = vec3(vertexes[3 * indices[i]],
                    vertexes[3 * indices[i] + 1],
                    vertexes[3 * indices[i] + 2]);

        t.v1 = vec3(vertexes[3 * indices[i + 1]],
                    vertexes[3 * indices[i + 1] + 1],
                    vertexes[3 * indices[i + 1] + 2]);

        t.v2 = vec3(vertexes[3 * indices[i + 2]],
                    vertexes[3 * indices[i + 2] + 1],
                    vertexes[3 * indices[i + 2] + 2]);

        t.u = t.v1 - t.v0;
        t.v = t.v2 - t.v0;
        t.normal = normalize(cross(t.u, t.v));
        triangles.push_back(t);
    }
}


Scene::~Scene()
{
    delete &vertexes;
    delete &indices;
    delete &normals;
}


void Scene::render(glm::mat4 &viewMatrix, glm::mat4 &projectionMatrix) {

    glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
    glm::mat4 modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;


    // Send uniforms to shader
    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "modelViewProjectionMatrix"), 1, false, &modelViewProjectionMatrix[0].x);
    glUniformMatrix4fv(glGetUniformLocation(shader, "modelViewMatrix"), 1, false, &modelViewMatrix[0].x);

    // Draw cube
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    int size;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0); //sizeof(GLushort),

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Scene::init() {
    // Shader setup
    shader = glHelper::loadShader(VERT_SHADER_PATH, FRAG_SHADER_PATH);
    GLuint mvp_location = glGetUniformLocation(shader, "MVP");
    GLuint vpos_location = glGetAttribLocation(shader, "vPos");

    // Scene setup
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexes.size(), &(vertexes[0]), GL_STATIC_DRAW);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLshort) * indices.size(), &(indices[0]), GL_STATIC_DRAW);

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}
