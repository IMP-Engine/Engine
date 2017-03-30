#include "ModelRenderer.h"

ModelRenderer::ModelRenderer()
{
}

ModelRenderer::~ModelRenderer()
{
}


void ModelRenderer::init()
{
    // Shader setup
    simpleShader = glHelper::loadShader(SURFACE_VERT_SHADER_PATH, SURFACE_FRAG_SHADER_PATH);
    GLuint mvp_location = glGetUniformLocation(simpleShader, "MVP");
    GLuint vpos_location = glGetAttribLocation(simpleShader, "vPos");
    GLuint nbo_location = glGetAttribLocation(simpleShader, "normal");

    glUseProgram(simpleShader);

    // Scene setup
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glGenBuffers(1, &nbo);
    glBindBuffer(GL_ARRAY_BUFFER, nbo);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    glEnableVertexAttribArray(nbo_location);
    glVertexAttribPointer(nbo_location, 3, GL_FLOAT, GL_TRUE, 0, (void*)0);

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void calculateVertices(ParticleData &particles, ModelData &data, std::vector<glm::vec3> &positions, std::vector<glm::vec3> &normals)
{
    for (int i = 0; i < data.closestParticles.size(); i += 3)
    {
        glm::vec3 p1 = particles.position[data.closestParticles[i]];
        glm::vec3 p2 = particles.position[data.closestParticles[i+1]];
        glm::vec3 p3 = particles.position[data.closestParticles[i+2]];
        
        glm::vec3 u = data.bcCoords[i]   * (p3 - p1);
        glm::vec3 v = data.bcCoords[i+1] * (p2 - p1);
        glm::vec3 normal = normalize(cross(u, v));

        positions.push_back(p1 + u + v + normal * data.bcCoords[i + 2]);
        //printf("Pos %i: {%f, %f, %f}\n", (i/3), positions[i/3].x, positions[i/3].y, positions[i/3].z);
        normals.push_back(normal);
    }
}

void ModelRenderer::render(ParticleData &particles, ModelData &data, glm::mat4 &modelViewProjectionMatrix, glm::mat4 &modelViewMatrix, glm::vec3 &viewSpaceLightPosition, glm::mat4 &projectionMatrix)
{
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    calculateVertices(particles, data, positions, normals);

    glUseProgram(simpleShader);
    glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), &(positions[0]), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, nbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), &(normals[0]), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(short) * data.elements.size(), &(data.elements[0]), GL_STATIC_DRAW);

    // Send uniforms to shader
	glUniformMatrix4fv(glGetUniformLocation(simpleShader, "modelViewProjectionMatrix"), 1, false, &modelViewProjectionMatrix[0].x);
	glUniformMatrix4fv(glGetUniformLocation(simpleShader, "modelViewMatrix"), 1, false, &modelViewMatrix[0].x);
	glUniformMatrix4fv(glGetUniformLocation(simpleShader, "projectionM4atrix"), 1, false, &projectionMatrix[0].x);
	glUniform3fv(glGetUniformLocation(simpleShader, "viewSpaceLightPos"), 1, &viewSpaceLightPosition.x);

    // Draw
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    int size;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
	//glDrawArrays(GL_TRIANGLES, 0, data.cardinality);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}
