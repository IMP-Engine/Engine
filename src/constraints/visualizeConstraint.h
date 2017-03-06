#pragma once
#include "Constraint.h"
#include "../particles/Particle.h"
#include <glm/vec3.hpp>
#include <vector>
#include <GLFW/glfw3.h>

#ifdef _WIN32
#define VERT_CONSTRAINT_SHADER_PATH "../../src/shaders/simple.vert"
#define FRAG_CONSTRAINT_SHADER_PATH "../../src/shaders/simple.frag"
#elif __unix__
#define VERT_CONSTRAINT_SHADER_PATH "../src/shaders/simple.vert"
#define FRAG_CONSTRAINT_SHADER_PATH "../src/shaders/simple.frag"
#endif

namespace visualization {

	GLuint constraintShader;
	std::vector<vec3> pos;

	void initialize() {
		constraintShader = glHelper::loadShader(VERT_CONSTRAINT_SHADER_PATH, FRAG_CONSTRAINT_SHADER_PATH);
	}

	void drawConstraints(std::vector<Constraint*>* constraints, mat4 mvp) {
		
		glUseProgram(constraintShader);
		glUniformMatrix4fv(glGetUniformLocation(simpleShader, "modelViewProjectionMatrix"), 1, false, &mvp[0].x);
		pos.clear();
		for (auto c : *constraints)
		{
			for(auto p : c->particles)
			{ 
				pos.push_back(p->pos);
			}
		}

		GLuint pos_buf;

		glGenBuffers(1, &pos_buf);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pos_buf);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pos), &pos, GL_DYNAMIC_DRAW);

		glDrawArrays(GL_LINES, 0, pos.size() / 3);
		glUseProgram(0);
	}

}
