#include "ParticleRenderer.h"




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

	glUseProgram(particleShader);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &particleBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, particleBuffer);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(positionAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(phaseAttribLocation, 1, GL_INT, GL_FALSE, sizeof(Particle), (void *)(sizeof(Particle) - sizeof(int)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void ParticleRenderer::render(mat4 &modelViewProjectionMatrix, mat4 &modelViewMatrix, vec3 &viewSpaceLightPosition, mat4 &projectionMatrix)
{
	glBindBuffer(GL_ARRAY_BUFFER, particleBuffer);
	Particle *ps = &(*particles)[0];
	glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * particles->size(), ps, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(particleShader);
	glBindVertexArray(vao);
	
	glUniformMatrix4fv(glGetUniformLocation(particleShader, "modelViewProjectionMatrix"), 1, false, &modelViewProjectionMatrix[0].x);
	glUniformMatrix4fv(glGetUniformLocation(particleShader, "modelViewMatrix"), 1, false, &modelViewMatrix[0].x);
	glUniformMatrix4fv(glGetUniformLocation(particleShader, "projectionM4atrix"), 1, false, &projectionMatrix[0].x);
	glUniform3fv(glGetUniformLocation(particleShader, "viewSpaceLightPos"), 1, &viewSpaceLightPosition.x);

	glDrawArrays(GL_POINTS, 0, particles->size());

	glBindVertexArray(0);
	glUseProgram(0);

}
