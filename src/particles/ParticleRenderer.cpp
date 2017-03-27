#include "ParticleRenderer.h"




ParticleRenderer::ParticleRenderer()
{
}

ParticleRenderer::~ParticleRenderer()
{
}


void ParticleRenderer::init()
{
	glEnable(GL_POINT_SPRITE);

	/*
	If GL_PROGRAM_POINT_SIZE is enabled, then the point size comes from the output variable float gl_PointSize.
	If it is disabled, the point size is constant for all points in a primitive, and is set by the glPointSize function.
	*/
	//glEnable(GL_PROGRAM_POINT_SIZE);

	/*
	If enabled and a vertex shader is active, then the derived point size is taken from the (potentially clipped)
	shader builtin gl_PointSize and clamped to the implementation-dependent point size range.
	*/
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

	particleShader = glHelper::loadShader(PARTICLE_VERT_SHADER_PATH, PARTICLE_FRAG_SHADER_PATH);

	int positionAttribLocation = glGetAttribLocation(particleShader, "position");
	int phaseAttribLocation = glGetAttribLocation(particleShader, "phase");
    int sizeAttribLocation = glGetAttribLocation(particleShader, "particleSize");

	glUseProgram(particleShader);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &particleBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, particleBuffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(positionAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);

    glGenBuffers(1, &sizeBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, sizeBuffer);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(sizeAttribLocation, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void *)0);

    /*
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(phaseAttribLocation, 1, GL_INT, GL_FALSE, sizeof(int), (void *)(sizeof(Particle) - sizeof(int)));
    */

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void ParticleRenderer::render(ParticleData &particles, glm::mat4 &modelViewProjectionMatrix, glm::mat4 &modelViewMatrix, glm::vec3 &viewSpaceLightPosition, glm::mat4 &projectionMatrix, GLint height)
{
	glBindBuffer(GL_ARRAY_BUFFER, particleBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * particles.cardinality, &particles.position[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, sizeBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * particles.cardinality, &particles.radius[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(particleShader);
	glBindVertexArray(vao);
	
	glUniformMatrix4fv(glGetUniformLocation(particleShader, "modelViewProjectionMatrix"), 1, false, &modelViewProjectionMatrix[0].x);
	glUniformMatrix4fv(glGetUniformLocation(particleShader, "modelViewMatrix"), 1, false, &modelViewMatrix[0].x);
	glUniformMatrix4fv(glGetUniformLocation(particleShader, "projectionM4atrix"), 1, false, &projectionMatrix[0].x);
	glUniform3fv(glGetUniformLocation(particleShader, "viewSpaceLightPos"), 1, &viewSpaceLightPosition.x);
    glUniform1i(glGetUniformLocation(particleShader, "height"), height);

	glDrawArrays(GL_POINTS, 0, particles.cardinality);

	glBindVertexArray(0);
	glUseProgram(0);

}
