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


#ifdef _WIN32
#define PARTICLE_VERT_SHADER_PATH "../../src/shaders/particle.vert.glsl"
#define PARTICLE_FRAG_SHADER_PATH "../../src/shaders/particle.frag.glsl"
#elif __unix__
#define PARTICLE_VERT_SHADER_PATH "../src/shaders/particle.vert.glsl"
#define PARTICLE_FRAG_SHADER_PATH "../src/shaders/particle.frag.glsl"
#endif


#include "Box.h"

#include "glHelper.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
using namespace glm;
using namespace std;

#include <vector>

// Global variables
GLFWwindow* window;
GLint mvp_location, vpos_location, vcol_location;

// Shaders
GLuint simpleShader, particleShader;

// Light
float point_light_intensity_multiplier = 1000.0f;
vec3 point_light_color = {1.f, 1.f, 1.f};
const vec3 lightPosition = {20.0f, 40.0f, 0.0f};

Box *box;


static const struct {
	float x, y;
	float r, g, b;
}

vertices[3] = {
	{ -0.6f, -0.4f, 1.f, 0.f, 0.f },
	{ 0.6f, -0.4f, 0.f, 1.f, 0.f },
	{ 0.f,  0.6f, 0.f, 0.f, 1.f }
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


void initParticleShader() {
	particleShader = glHelper::loadShader(PARTICLE_VERT_SHADER_PATH, PARTICLE_FRAG_SHADER_PATH);

	glBindAttribLocation(particleShader, 0, "position");

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed to initialize OpenGL context");
		return;
	}

	glUseProgram(particleShader);
	// Triangle setup
	GLuint particleBuffer;

	Particle *particles = &(box->particles)[0];
	glGenBuffers(1, &particleBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, particleBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particles), particles, GL_STATIC_DRAW);

	//int location = glGetAttribLocation(particleShader, "position");
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *)0);

	/*

	location = glGetAttribLocation(particleShader, "phase");
	glEnableVertexAttribArray(location);
	glVertexAttribPointer(location, 1, GL_INT, GL_FALSE, sizeof(Particle), (void *)(sizeof(Particle) - sizeof(int)));
	*/

}

void initGL() {

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window = glfwCreateWindow(1280, 800, "Simple example", NULL, NULL);
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
	simpleShader = glHelper::loadShader(VERT_SHADER_PATH, FRAG_SHADER_PATH);
	glUseProgram(simpleShader);
	mvp_location = glGetUniformLocation(simpleShader, "MVP");
	glBindAttribLocation(simpleShader, 0, "vPos");
	glBindAttribLocation(simpleShader, 1, "vCol");
	//vpos_location = glGetAttribLocation(simpleShader, "vPos");
	//vcol_location = glGetAttribLocation(simpleShader, "vCol");
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

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 2));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	initParticleShader();

	glPointSize(30.f);

}


void renderParticles() {
	glUseProgram(particleShader);

	glDrawArrays(GL_POINTS, 0, box->particles.size());

	glUseProgram(0);
}

void display() {
	float ratio;
	int width, height;
	mat4 m, p, mvp;

	glfwGetFramebufferSize(window, &width, &height);
	ratio = width / (float)height;

	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT);

	m = { 1.f, 0.f, 0.f, 0.f,
		  0.f, 1.f, 0.f, 0.f,
		  0.f, 0.f, 1.f, 0.f,
		   0.f, 0.f, 0.f, 1.f
		};

	m = rotate((float)glfwGetTime(), vec3(0, 0, 1)) * m;
	p = ortho(-1.f, 1.f, 1.f, -1.f);
	mvp = p * m;

	glUseProgram(simpleShader);
	glUniformMatrix4fv(glGetUniformLocation(simpleShader, "MVP"), 1, false, &mvp[0].x);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	//renderParticles();

	ImGui::Render();

	glfwSwapBuffers(window);
}

int main(void) {


	BoxConfig config;

	config.dimensions =	vec3(10.f, 10.f, 10.f);
	config.center_pos = vec3(5.f, 5.f, 5.f);
	config.mass = 10.f;
	config.phase = 1;
	config.num_particles = vec3(3,3,3);

	box = make_box(&config);





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

