#pragma once
#include "../particles/Particle.h"
#include <glm/vec3.hpp>
#include <vector>
#include <GLFW/glfw3.h>

#ifdef _WIN32
#define VERT_CONSTRAINT_SHADER_PATH "../../src/shaders/simpleLine.vert"
#define FRAG_CONSTRAINT_SHADER_PATH "../../src/shaders/simpleLine.frag"
#elif __unix__
#define VERT_CONSTRAINT_SHADER_PATH "../src/shaders/simpleLine.vert"
#define FRAG_CONSTRAINT_SHADER_PATH "../src/shaders/simpleLine.frag"
#endif

namespace visualization {

	GLuint constraintShader;
	std::vector<vec3> pos;
	GLuint pos_buf;
	bool draw = false;

	void initialize() {
		constraintShader = glHelper::loadShader(VERT_CONSTRAINT_SHADER_PATH, FRAG_CONSTRAINT_SHADER_PATH);
		glGenBuffers(1, &pos_buf);
	}

	void gui() {
		ImGui::Checkbox("Visualize constraints", &draw);
	}

	// Currently only handles distance constraints
	void drawConstraints(ConstraintData &constraints, ParticleData particles, mat4 mvp) {
		
		if (!draw || !constraints.distanceConstraints.cardinality)
			return;

		// Copy positions into separate vector to avoid uploading particles into GPU
        std::vector<vec3> &position = particles.position;
		pos.clear();
		for (int i = 0;i < constraints.distanceConstraints.cardinality; i++)
		{
            pos.push_back(position[constraints.distanceConstraints.particles[i].x]);
            pos.push_back(position[constraints.distanceConstraints.particles[i].y]);
		}

		glUseProgram(constraintShader);
		glUniformMatrix4fv(glGetUniformLocation(constraintShader, "modelViewProjectionMatrix"), 1, false, &mvp[0].x);

		glBindBuffer(GL_ARRAY_BUFFER, pos_buf);
		glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(vec3), &pos[0], GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		//glEnable(GL_LINE_SMOOTH);
		//glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

		//glLineWidth(2.f);

		glDrawArrays(GL_LINES, 0, pos.size());
		//glDisableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		glUseProgram(0);
	}

}
