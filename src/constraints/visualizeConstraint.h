#pragma once
#include "Constraint.h"
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

	void initialize() {
		constraintShader = glHelper::loadShader(VERT_CONSTRAINT_SHADER_PATH, FRAG_CONSTRAINT_SHADER_PATH);
		glGenBuffers(1, &pos_buf);
	}

	// Currently only handles distance constraints
	void drawConstraints(std::vector<Constraint*>* constraints, mat4 mvp) {
		
		// Copy positions into separate vector to avoid uploading particles into GPU
		pos.clear();
		for (auto c : *constraints)
		{
			for(auto p : c->particles)
			{ 
				pos.push_back(p->pos);
			}
		}

		glUseProgram(constraintShader);
		glUniformMatrix4fv(glGetUniformLocation(constraintShader, "modelViewProjectionMatrix"), 1, false, &mvp[0].x);

		glBindBuffer(GL_ARRAY_BUFFER, pos_buf);
		glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(vec3), &pos.front(), GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		//glEnable(GL_LINE_SMOOTH);
		//glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

		//glLineWidth(2.f);

		glDrawArrays(GL_LINES, 0, pos.size());
		//glDisableVertexAttribArray(0);
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		glUseProgram(0);
	}

}
