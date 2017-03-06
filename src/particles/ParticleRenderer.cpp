#include "ParticleRenderer.h"




ParticleRenderer::~ParticleRenderer()
{
}


void ParticleRenderer::init()
{

	particleShader = glHelper::loadShader(PARTICLE_VERT_SHADER_PATH, PARTICLE_FRAG_SHADER_PATH);

	int positionAttribLocation = glGetAttribLocation(particleShader, "position");
	int phaseAttribLocation = glGetAttribLocation(particleShader, "phase");

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed to initialize OpenGL context");
		return;
	}

	glUseProgram(particleShader);


	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	Particle *ps = &(*particles)[0];
	glGenBuffers(1, &particleBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, particleBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * particles->size(), ps, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(positionAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(phaseAttribLocation, 1, GL_INT, GL_FALSE, sizeof(Particle), (void *)(sizeof(Particle) - sizeof(int)));


}
void ParticleRenderer::render(mat4 &modelViewProjectionMatrix, mat4 &modelViewMatrix, vec3 &viewSpaceLightPosition, mat4 &projectionMatrix)
{
	glUseProgram(particleShader);

	glBindVertexArray(vao);
	Particle *ps = &(*particles)[0];
	glBindBuffer(GL_ARRAY_BUFFER, particleBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * particles->size(), ps, GL_STATIC_DRAW);

	glUniformMatrix4fv(glGetUniformLocation(particleShader, "modelViewProjectionMatrix"), 1, false, &modelViewProjectionMatrix[0].x);
	glUniformMatrix4fv(glGetUniformLocation(particleShader, "modelViewMatrix"), 1, false, &modelViewMatrix[0].x);
	glUniformMatrix4fv(glGetUniformLocation(particleShader, "projectionM4atrix"), 1, false, &projectionMatrix[0].x);
	glUniform3fv(glGetUniformLocation(particleShader, "viewSpaceLightPos"), 1, &viewSpaceLightPosition.x);

	glDrawArrays(GL_POINTS, 0, particles->size());

	glUseProgram(0);

}
