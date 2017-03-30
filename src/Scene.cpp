#include "Scene.h"



Scene::Scene()
{
    // Cube setup
    unsigned short cubeIndices[] = {
        // front
        2, 1, 0,
        2, 0, 3,
        // top
        6, 5, 4,
        6, 4, 7,
        // back
        10, 9, 8,
        10, 8, 11,
        // bottom
        14, 13, 12,
        14, 12, 15,
        // left
        18, 17, 16,
        18, 16, 19,
        // right
        22, 21, 20,
        22, 20, 23
    };

    float cubeVertices[] = {
        // Front
        -10.0f, -10.0f, 10.0f, // lower left close
         10.0f, -10.0f, 10.0f, // lower right close
         10.0f,  10.0f, 10.0f, // upper right close
        -10.0f,  10.0f, 10.0f, // upper left close
        // Top
        -10.0f, 10.0f,  10.0f, // upper left close
         10.0f, 10.0f,  10.0f, // upper right close
         10.0f, 10.0f, -10.0f, // upper right borta
        -10.0f, 10.0f, -10.0f, // upper left borta
        // Back
        -10.0f,  10.0f, -10.0f, // upper left borta
         10.0f,  10.0f, -10.0f, // upper right borta
         10.0f, -10.0f, -10.0f, // lower right borta
        -10.0f, -10.0f, -10.0f, // lower left borta
        // Bottom
        -10.0f, -10.0f, -10.0f, // lower left borta
         10.0f, -10.0f, -10.0f, // lower right borta
         10.0f, -10.0f,  10.0f, // lower right close
        -10.0f, -10.0f,  10.0f, // lower left close
        // Left
        -10.0f, -10.0f, -10.0f, // lower left borta
        -10.0f, -10.0f,  10.0f, // lower left close
        -10.0f,  10.0f,  10.0f, // upper left close
        -10.0f,  10.0f, -10.0f, // upper left borta
        // Right
        10.0f, -10.0f,  10.0f, // lower right close
        10.0f, -10.0f, -10.0f, // lower right borta
        10.0f,  10.0f, -10.0f, // upper right borta
        10.0f,  10.0f,  10.0f // upper right close
    };

    float normalComponents[] = {
        // Front
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        // Top
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        // Back
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        // Bottom
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        // Left
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        // Right
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f
    };

    for (int i = 0; i < 24*3; i++)
    {
        vertexes.push_back(cubeVertices[i]);
    }
    for (int i = 0; i < 36; i++)
    {
        indices.push_back(cubeIndices[i]);
    }
    for (int i = 0; i < 24*3; i++)
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

void Scene::render(glm::mat4 &viewMatrix, glm::mat4 &projectionMatrix, const glm::vec3 &lightPosition) {

    glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
    glm::mat4 modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

    // Send uniforms to shader
    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "modelViewProjectionMatrix"), 1, false, &modelViewProjectionMatrix[0].x);
    glUniformMatrix4fv(glGetUniformLocation(shader, "modelViewMatrix"), 1, false, &modelViewMatrix[0].x);
    glUniform3fv(glGetUniformLocation(shader, "lightPos"), 1, &lightPosition.x);

    // Draw cube
    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_LINE);

    glBindVertexArray(vao);
   
    int size;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0); //sizeof(GLushort),

	glBindVertexArray(0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_CULL_FACE);
}

void Scene::init() {

    // Shader setup
    shader = glHelper::loadShader(VERT_SHADER_PATH, FRAG_SHADER_PATH);
    GLuint mvp_location = glGetUniformLocation(shader, "MVP");
    GLuint vpos_location = glGetAttribLocation(shader, "vPos");
    GLuint normal_location = glGetAttribLocation(shader, "normal");

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

    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normals.size(), &(normals[0]), GL_STATIC_DRAW);

    glEnableVertexAttribArray(normal_location);
    glVertexAttribPointer(normal_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
