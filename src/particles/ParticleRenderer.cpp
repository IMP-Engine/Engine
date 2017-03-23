#include "ParticleRenderer.h"




ParticleRenderer::ParticleRenderer()
{
}

ParticleRenderer::~ParticleRenderer()
{
}


void ParticleRenderer::init(ParticleData particles)
{

    particleCount  = &particles.cardinality;
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

    glm::vec3 *firstElement = &particles.position[0];

	glGenBuffers(1, &particleBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, particleBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * particles.cardinality, firstElement, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(positionAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);

    /*
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(phaseAttribLocation, 1, GL_INT, GL_FALSE, sizeof(int), (void *)(sizeof(Particle) - sizeof(int)));
    */

	glBindVertexArray(0);
}
void ParticleRenderer::render(glm::mat4 &modelViewProjectionMatrix, glm::mat4 &modelViewMatrix, glm::vec3 &viewSpaceLightPosition, glm::mat4 &projectionMatrix)
{
	glUseProgram(particleShader);

	glBindVertexArray(vao);

	glUniformMatrix4fv(glGetUniformLocation(particleShader, "modelViewProjectionMatrix"), 1, false, &modelViewProjectionMatrix[0].x);
	glUniformMatrix4fv(glGetUniformLocation(particleShader, "modelViewMatrix"), 1, false, &modelViewMatrix[0].x);
	glUniformMatrix4fv(glGetUniformLocation(particleShader, "projectionM4atrix"), 1, false, &projectionMatrix[0].x);
	glUniform3fv(glGetUniformLocation(particleShader, "viewSpaceLightPos"), 1, &viewSpaceLightPosition.x);

	glDrawArrays(GL_POINTS, 0, (*particleCount));

	glBindVertexArray(0);
	glUseProgram(0);

}
