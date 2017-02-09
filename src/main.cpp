//#include <glad/glad.h>
#include "glad.h"
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

#include "Box.h"

#include "glHelper.h"
#include "linmath\float3.h"
#include "linmath\float4x4.h"
#include "linmath\linmath.h"
#include <vector>

using namespace chag;

// Global variables
GLFWwindow* window;
GLint mvp_location, vpos_location, vcol_location;

// Shaders
GLuint simpleShader;

// Light
float point_light_intensity_multiplier = 1000.0f;
float3 point_light_color = {1.f, 1.f, 1.f};
const float3 lightPosition = {20.0f, 40.0f, 0.0f};

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

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
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

	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

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

	glEnableVertexAttribArray(vpos_location);
	glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
		sizeof(float) * 5, (void*)0);
	glEnableVertexAttribArray(vcol_location);
	glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
		sizeof(float) * 5, (void*)(sizeof(float) * 2));

}

void display() {
	float ratio;
	int width, height;
	mat4x4 m, p, mvp;

	glfwGetFramebufferSize(window, &width, &height);
	ratio = width / (float)height;

	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT);

	mat4x4_identity(m);
	mat4x4_rotate_Z(m, m, (float)glfwGetTime());
	mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
	mat4x4_mul(mvp, p, m);

	glUseProgram(simpleShader);
	glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)mvp);
	glDrawArrays(GL_TRIANGLES, 0, 3);

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

	config.dimensions = make_vector<float>(10.f, 10.f, 10.f);
	config.center_pos = make_vector<float>(5.f, 5.f, 5.f);
	config.mass = 10.f;
	config.num_particles = make_vector<int>(3, 3, 3);

	Box *box = make_box(&config);

    while (!glfwWindowShouldClose(window)) {
		double timeElapsed = glfwGetTime() - startTime;
		display();

		glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
