#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"
#include <iostream>
#include "debug.h"


#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))


#include "Box.h"

#include "glHelper.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
using namespace glm;

#include <vector>

// Global variables
GLFWwindow* window;
GLint mvp_location, vpos_location, vcol_location;
GLuint cube_ibo;

// Shaders
GLuint simpleShader;

float vertices[] = {
	/*-10.f, -5.f, -10.f,
	 10.f, -5.f, -10.f,
	-10.f, -5.f,  10.f,

	-10.f, -5.f,  10.f,
	 10.f, -5.f, -10.f,
	 10.f, -5.f,  10.f*/

	-10.0f, -10.0f,  10.0f,
	 10.0f, -10.0f,  10.0f,
	 10.0f,  10.0f,  10.0f,
	-10.0f,  10.0f,  10.0f,

	-10.0f, -10.0f, -10.0f,
	 10.0f, -10.0f, -10.0f,
	 10.0f,  10.0f, -10.0f,
	-10.0f,  10.0f, -10.0f,
};

// Framebuffer
//struct FboInfo;
//std::vector<FboInfo> fboList;
//
//struct FboInfo {
//	GLuint framebufferId;
//	GLuint colorTextureTarget;
//	GLuint depthBuffer;
//	int width;
//	int height;
//	bool isComplete;
//
//	FboInfo(int w, int h) {
//		isComplete = false;
//		width = w;
//		height = h;
//
//		// Generate two textures and set filter parameters (no storage allocated yet)
//		glGenTextures(1, &colorTextureTarget);
//		glBindTexture(GL_TEXTURE_2D, colorTextureTarget);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//		glGenTextures(1, &depthBuffer);
//		glBindTexture(GL_TEXTURE_2D, depthBuffer);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//		// allocate storage for textures
//		resize(width, height);
//
//		// Generate and bind framebuffer
//		// check if framebuffer is complete
//		glGenFramebuffers(1, &framebufferId);
//		glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
//
//		// bind the texture as color attachment 0 (to the currently bound framebuffer)
//		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTextureTarget, 0);
//		glDrawBuffer(GL_COLOR_ATTACHMENT0);
//
//		// bind the texture as depth attachment (to the currently bound framebuffer)
//		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer, 0);
//
//		isComplete = checkFramebufferComplete();
//
//		// bind default framebuffer, just in case.
//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	}
//
//	// if no resolution provided
//	FboInfo() : isComplete(false)
//		, framebufferId(UINT32_MAX)
//		, colorTextureTarget(UINT32_MAX)
//		, depthBuffer(UINT32_MAX)
//		, width(0)
//		, height(0)
//	{};
//
//	void resize(int w, int h) {
//		width = w;
//		height = h;
//		// Allocate a texture
//		glBindTexture(GL_TEXTURE_2D, colorTextureTarget);
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
//
//		// generate a depth texture
//		glBindTexture(GL_TEXTURE_2D, depthBuffer);
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
//	}
//
//	bool checkFramebufferComplete(void) {
//		// Check that our FBO is correctly set up, this can fail if we have
//		// incompatible formats in a buffer, or for example if we specify an
//		// invalid drawbuffer, among things.
//		glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
//		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
//		if (status != GL_FRAMEBUFFER_COMPLETE) {
//			//glHelper::fatal_error("Framebuffer not complete");
//			// Error
//		}
//
//		return (status == GL_FRAMEBUFFER_COMPLETE);
//	}
//};
// ************************ End Framebuffer *************************

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

	glfwSwapInterval(1); // v-sync

	// NOTE: OpenGL error checks have been omitted for brevity

	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

	// Shader setup
	simpleShader = glHelper::loadShader("../../src/shaders/simple.vert", "../../src/shaders/simple.frag");
	mvp_location = glGetUniformLocation(simpleShader, "MVP");
	vpos_location = glGetAttribLocation(simpleShader, "vPos");
	vcol_location = glGetAttribLocation(simpleShader, "vCol");


	// Framebuffer setup
	/*int w, h;
	glfwGetWindowSize(window, &w, &h);
	const int numFbos = 5;
	for (int i = 0; i < numFbos; i++)
		fboList.push_back(FboInfo(w, h));*/

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed to initialize OpenGL context");
		return;
	}

	// Triangle setup
	GLuint vertex_buffer;

	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//glEnableVertexAttribArray(vpos_location);
	//glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
	//	/*sizeof(float) * 3, (void*)0);*/
	//	0, (void*)0);

	// Cube setup
	GLushort indices[] = {
		//// front
		//2, 1, 0,
		//0, 3, 2,
		//// top
		//6, 5, 1,
		//1, 2, 6,
		//// back
		//5, 6, 7,
		//7, 4, 5,
		//// bottom
		//3, 0, 4,
		//4, 7, 3,
		//// left
		//1, 5, 4,
		//4, 0, 1,
		//// right
		//6, 2, 3,
		//3, 7, 6

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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(vpos_location);
	glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
		/*sizeof(float) * 3, (void*)0);*/
		0, (void*)0);

	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

void display() {
	float ratio;
	int width, height;
	mat4 v, p, mvp, mv;

	glfwGetFramebufferSize(window, &width, &height);
	ratio = width / (float)height;

	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 m(1.0f); // Identity matrix

	//m = rotate((float)glfwGetTime(), vec3(0, 0, 1)) * m;
	v = lookAt(vec3(20, 4, 40), vec3(0), vec3(0, 1, 0));

	// Set up a projection matrix
	float fovy = radians(45.0f);
	float nearPlane = 0.01f;
	float farPlane = 300.0f;

	p = perspective(fovy, ratio, nearPlane, farPlane); //ortho(-1.f, 1.f, 1.f, -1.f);
	mv = v * m;
	mvp = p * mv;

	// Send uniforms to shader
	glUseProgram(simpleShader);
	glUniformMatrix4fv(glGetUniformLocation(simpleShader, "MVP"), 1, false, &mvp[0].x);
	glUniformMatrix4fv(glGetUniformLocation(simpleShader, "MV"), 1, false, &mv[0].x);

	// Draw
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ibo);
	int size;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0); //sizeof(GLushort),

	//glDrawArrays(GL_TRIANGLES, 0, 3);
	//glDrawArrays(GL_TRIANGLES, 3, 3);


  ImGui::Render();

	glfwSwapBuffers(window);
}

int main(void) {
	initGL();

	printf("OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);

	//if (GLAD_GL_EXT_framebuffer_multisample) {
	//	/* GL_EXT_framebuffer_multisample is supported */
	//}
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

    

    while (!glfwWindowShouldClose(window))
    {
		glfwPollEvents();
		ImGui_ImplGlfwGL3_NewFrame();


		ImVec4 clear_color = ImColor(164, 164, 164);

		// Consider scapping incase of performance
		static bool vsync = true;
		glfwSwapInterval(vsync ? 1 : 0);

		{ // GUI
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

        display();
    }

	// Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();
	ImGui_ImplGlfwGL3_Shutdown();
    exit(EXIT_SUCCESS);
}

