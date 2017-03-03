#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <string>

#include "glHelper.h"
#include <string>
#include <iostream>
//#include <sstream>
#include <fstream>
#include <vector>

using namespace std;

namespace glHelper {

	// Reads, compiles, links and returns a shader from the given paths
	GLuint loadShader(const std::string &vertexPath, const std::string &fragmentPath) {

		printf("Loading shader program with shaders:\n");
		cout << "    Vertex:   " << vertexPath << endl;
		cout << "    Fragment: " << fragmentPath << endl;

		// Read our shaders into the appropriate buffers
		std::ifstream vs_file(vertexPath);
		std::string vertexSource((std::istreambuf_iterator<char>(vs_file)), std::istreambuf_iterator<char>());

		std::ifstream fs_file(fragmentPath);
		std::string fragmentSource((std::istreambuf_iterator<char>(fs_file)), std::istreambuf_iterator<char>());

		//const char *vs = vertexSource.c_str();
		//const char *fs = fragmentSource.c_str();

        //cout << vs << endl << fs << endl;

		// Create an empty vertex shader handle
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		// Send the vertex shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		const GLchar *source = (const GLchar *)vertexSource.c_str();
		glShaderSource(vertexShader, 1, &source, NULL);

		// Compile the vertex shader
		glCompileShader(vertexShader);

		GLint isCompiled = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(vertexShader);

			// Time to use the infoLog.
			for (unsigned int i = 0; i < infoLog.size(); i++)
			{
				std::cerr << infoLog[i];
			}
			std::cerr << endl;


			// In this simple program, we'll just leave
			return -1;
		}

		// Create an empty fragment shader handle
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		// Send the fragment shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		source = (const GLchar *)fragmentSource.c_str();
		glShaderSource(fragmentShader, 1, &source, NULL);

		// Compile the fragment shader
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(fragmentShader);
			// Either of them. Don't leak shaders.
			glDeleteShader(vertexShader);

			// Time to use the infoLog.
			for (unsigned int i = 0; i < infoLog.size(); i++)
			{
				std::cerr << infoLog[i];
			}
			std::cerr << endl;
			// In this simple program, we'll just leave
			return -1;
		}

		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.
		GLuint program = glCreateProgram();

		// Attach our shaders to our program
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);

		// Link our program
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);

		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(program);
			// Don't leak shaders either.
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			// Time to use the infoLog.
			for (unsigned int i = 0; i < infoLog.size(); i++)
			{
				std::cerr << infoLog[i];
			}
			std::cerr << endl;
            
			// In this simple program, we'll just leave
			return -1;
		}

		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		if (maxLength > 0) {

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
			for (unsigned int i = 0; i < infoLog.size(); i++)
			{
				std::cerr << infoLog[i];
			}
			std::cerr << endl;
		
		}
		// Detach shaders after a successful link.
		glDetachShader(program, vertexShader);
		glDetachShader(program, fragmentShader);


		printf("Shader loaded.\n");
		return program;
	}
}
