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

	GLuint particleBuffer;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	Particle *particles = &(*ParticleRenderer::particles)[0];
	glGenBuffers(1, &particleBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, particleBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * ParticleRenderer::particles->size(), particles, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(positionAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(phaseAttribLocation, 1, GL_INT, GL_FALSE, sizeof(Particle), (void *)(sizeof(Particle) - sizeof(int)));


}
void ParticleRenderer::render(mat4 &modelViewProjectionMatrix)
{
	glUseProgram(particleShader);

	glBindVertexArray(vao);

	glUniformMatrix4fv(glGetUniformLocation(particleShader, "modelViewProjectionMatrix"), 1, false, &modelViewProjectionMatrix[0].x);

	glDrawArrays(GL_POINTS, 0, particles->size());

	glUseProgram(0);

}
