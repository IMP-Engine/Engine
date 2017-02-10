#include "debug.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>
#include <sstream>
#include <stdio.h>

namespace debug {

void printGLDiagnostics()
{
	// print diagnostic information
	std::cout << "GL VENDOR: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "VERSION:   " << glGetString(GL_VERSION) << std::endl;
	std::cout << "RENDERER:  " << glGetString(GL_RENDERER) << std::endl;
	//std::cout << "EXTENSIONS:" << glGetString(GL_EXTENSIONS) << std::endl;
}

namespace
{
#	if defined(_WIN32)
#		define CALLBACK_ CALLBACK
#	else // !win32
#		define CALLBACK_
#	endif // ~ platform

	GLvoid CALLBACK_ handle_debug_message_(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, GLvoid* /*user*/)
	{
		// source string
		const char* srcStr = 0;
		switch (source) {
		case GL_DEBUG_SOURCE_API:				srcStr = "API";	break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:		srcStr = "WINDOW_SYSTEM"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:	srcStr = "SHADER_COMPILER"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:		srcStr = "THIRD_PARTY"; break;
		case GL_DEBUG_SOURCE_APPLICATION:		srcStr = "APPLICATION"; break;
		case GL_DEBUG_SOURCE_OTHER:				srcStr = "OTHER"; break;
		default:								srcStr = "UNKNOWN";	break;
		}

		// type
		const char* typeStr = 0;
		switch (type) {
		case GL_DEBUG_TYPE_ERROR:				typeStr = "ERROR";	break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:	typeStr = "DEPRECATED_BEHAVIOR";	break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:	typeStr = "UNDEFINED_BEHAVIOR";	break;
		case GL_DEBUG_TYPE_PORTABILITY:			typeStr = "PORTABILITY";	break;
		case GL_DEBUG_TYPE_PERFORMANCE:			typeStr = "PERFORMANCE";	break;
		case GL_DEBUG_TYPE_OTHER:				typeStr = "OTHER"; break;
		default:								typeStr = "UNKNOWN";	break;
		}

		// severity
		const char* sevStr = 0;
		switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH:			sevStr = "HIGH"; break;
		case GL_DEBUG_SEVERITY_MEDIUM:			sevStr = "MEDIUM"; break;
		case GL_DEBUG_SEVERITY_LOW:				sevStr = "LOW";	break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:	sevStr = "NOTIFICATION"; break;
		default:								sevStr = "UNKNOWN";
		}

		// output message, if not just notification
		std::stringstream szs;
		szs << "\n"
			<< "--\n"
			<< "-- GL DEBUG MESSAGE:\n"
			<< "--   severity = '" << sevStr << "'\n"
			<< "--   type     = '" << typeStr << "'\n"
			<< "--   source   = '" << srcStr << "'\n"
			<< "--   id       = " << std::hex << id << "\n"
			<< "-- message:\n"
			<< message << "\n"
			<< "--\n"
			<< "\n"
			;

		fprintf(stderr, "%s", szs.str().c_str());
		fflush(stderr);
	}
#	undef CALLBACK_
}

void setupGLDebugMessages() {

	if (!glfwExtensionSupported("GL_ARB_debug_output"))
	{
		std::cerr << "GL_debug_ouptput not suppoerted on this device" << std::endl;
		return;
	}

	if (glDebugMessageCallback) {
		/* This causes a distinc performance loss but allows
		*  the callback to be called immediately on error.
		*/
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback((GLDEBUGPROC)handle_debug_message_, 0);
		glDebugMessageControl(GL_DONT_CARE,
			GL_DONT_CARE,
			GL_DONT_CARE,
			0,
			0,
			true);
	}
	else
	{
		std::cerr << "glDebugMessageCallback not available" << std::endl;
	}
}
}
