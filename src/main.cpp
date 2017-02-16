#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"
#include <iostream>
#include "debug.h"


#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

#ifdef _WIN32
    #define VERT_SHADER_PATH "../../src/shaders/simple.vert"
    #define FRAG_SHADER_PATH "../../src/shaders/simple.frag"
#elif __unix__
    #define VERT_SHADER_PATH "../src/shaders/simple.vert"
    #define FRAG_SHADER_PATH "../src/shaders/simple.frag"
#endif


#include "Box.h"

#include "glHelper.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
using namespace glm;

#include <vector>

// Global variables
GLFWwindow* window;
GLint mvp_location, vpos_location, vcol_location;
GLuint cube_ibo; // IndicesBufferObject
ImVec4 clear_color;

// Shaders
GLuint simpleShader;

float cubeVertices[] = {
	-10.0f, -10.0f,  10.0f,
	 10.0f, -10.0f,  10.0f,
	 10.0f,  10.0f,  10.0f,
	-10.0f,  10.0f,  10.0f,

	-10.0f, -10.0f, -10.0f,
	 10.0f, -10.0f, -10.0f,
	 10.0f,  10.0f, -10.0f,
	-10.0f,  10.0f, -10.0f,
};

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void initGL() {
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
#if _DEBUG
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

	if (!window) {
		std::cout << "Failed to crate GLFW window" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize OpenGL context" << std::endl;
		exit(EXIT_FAILURE);
	}

#if _DEBUG		
	debug::printGLDiagnostics();
	debug::setupGLDebugMessages();
#endif

	ImGui_ImplGlfwGL3_Init(window, true); 

	// steal callback and call imgui in our callback
	glfwSetKeyCallback(window, key_callback);

	// Shader setup
	simpleShader = glHelper::loadShader(VERT_SHADER_PATH, FRAG_SHADER_PATH);
	mvp_location = glGetUniformLocation(simpleShader, "MVP");
	vpos_location = glGetAttribLocation(simpleShader, "vPos");
	vcol_location = glGetAttribLocation(simpleShader, "vCol");

	// Triangle setup
	GLuint vertex_buffer;

	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	// Cube setup
	GLushort cubeIndices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// top
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// bottom
		4, 0, 3,
		3, 7, 4,
		// left
		4, 5, 1,
		1, 0, 4,
		// right
		3, 2, 6,
		6, 7, 3
	};

	glGenBuffers(1, &cube_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(vpos_location);
	glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

void display() {
	float ratio;
	int width, height;
	mat4 viewMatrix, modelViewProjectionMatrix, modelViewMatrix;

	glfwGetFramebufferSize(window, &width, &height);
	ratio = width / (float)height;

	glViewport(0, 0, width, height);
	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 modelMatrix(1.0f); // Identity matrix

	viewMatrix = lookAt(vec3(20, 4, 40), vec3(0), vec3(0, 1, 0));

	// Set up a projection matrix
	float fovy = radians(45.0f);
	float nearPlane = 0.01f;
	float farPlane = 300.0f;

	modelViewMatrix = viewMatrix * modelMatrix;
	modelViewProjectionMatrix = perspective(fovy, ratio, nearPlane, farPlane) * modelViewMatrix;

	// Send uniforms to shader
	glUseProgram(simpleShader);
	glUniformMatrix4fv(glGetUniformLocation(simpleShader, "modelViewProjectionMatrix"), 1, false, &modelViewProjectionMatrix[0].x);
	glUniformMatrix4fv(glGetUniformLocation(simpleShader, "modelViewMatrix"), 1, false, &modelViewMatrix[0].x);

	// Draw
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ibo);
	int size;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0); //sizeof(GLushort),

	// GUI
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	ImGui::Render();

	glfwSwapBuffers(window);
}

void gui()
{
	// Consider scapping incase of performance
	static bool vsync = true;
	glfwSwapInterval(vsync ? 1 : 0);

	// Data for plotting frametime
	static float frameTimes[200] = { 0 };
	static int offset = 0;
	frameTimes[offset] = ImGui::GetIO().DeltaTime;
	offset = (offset + 1) % COUNT_OF(frameTimes);

	static bool show_demo_window = false;

	ImGui::Begin("IMPEngine");
	ImGui::ColorEdit3("clear color", (float*)&clear_color);
	ImGui::Checkbox("Vsync", &vsync);
	if (ImGui::Button("Demo Window")) show_demo_window ^= 1;
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::PlotLines("", frameTimes, COUNT_OF(frameTimes), offset, "Time/Frame [s]", FLT_MIN, FLT_MAX, ImVec2(0, 80));
	ImGui::End();

	// Demo window
	if (show_demo_window)
	{
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_demo_window);
	}
}

int main(void) {
	initGL();

	if (GLAD_GL_VERSION_4_3) {
		/* We support at least OpenGL version 4.3 */
	}

	double startTime = glfwGetTime();

	// Showcase of how the box works
	BoxConfig config;

	config.dimensions =	vec3(10.f, 10.f, 10.f);
	config.center_pos = vec3(5.f, 5.f, 5.f);
	config.mass = 10.f;
	config.phase = 1;
	config.num_particles = vec3(3,3,3);

	Box *box = make_box(&config);

	clear_color = ImColor(164, 164, 164);

    while (!glfwWindowShouldClose(window))
    {
		glfwPollEvents();
		ImGui_ImplGlfwGL3_NewFrame();

		gui();

        display();
    }

	// Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();
	ImGui_ImplGlfwGL3_Shutdown();
    exit(EXIT_SUCCESS);
}

