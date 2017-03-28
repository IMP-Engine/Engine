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
    simpleShader = glHelper::loadShader(VERT_SHADER_PATH, FRAG_SHADER_PATH);
    GLuint mvp_location = glGetUniformLocation(simpleShader, "MVP");
    GLuint vpos_location = glGetAttribLocation(simpleShader, "vPos");

    glUseProgram(simpleShader);

    // Scene setup
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void calculateVertices(ModelData &data, std::vector<glm::vec3> &positions, std::vector<glm::vec3> &normals)
{

}

void ModelRenderer::render(ModelData &data, glm::mat4 &modelViewProjectionMatrix, glm::mat4 &modelViewMatrix, glm::vec3 &viewSpaceLightPosition, glm::mat4 &projectionMatrix)
{
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    calculateVertices(data, positions, normals);

    glUseProgram(simpleShader);
    glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * data.cardinality/3, &(positions[0]), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, ibo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(short) * data.cardinality, &(data.elements[0]), GL_STATIC_DRAW);

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
