#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"
#include <iostream>
#include "debug.h"

// Example of path
#include "../tbb/include/tbb/parallel_for.h"

#include "performance.h"
#include "camera.h"

#include "glHelper.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>


#define WORLD_MIN vec3(-20.f,-20.f,-20.f)
#define WORLD_MAX vec3( 20.f, 20.f, 20.f)

#include "performance.h"
#include "physics.h"
#include "particles/ParticleRenderer.h"
#include "Scene.h"

#include "constraints/visualizeConstraint.h"
#include "models/model.h"
#include "input.h"

#ifdef _DEBUG
#include "intersections/tests.h"
#endif // _DEB


using namespace glm;
using namespace std;

/**************************************************************************
 ********************** Global variables **********************************
 **************************************************************************/

 // Application
GLFWwindow* window;
ImVec4 clear_color = ImColor(255, 255, 255);;
bool vsync = true;
const GLuint WIDTH = 1280, HEIGHT = 720;

// Camera
Camera camera;

// Deltatime
GLdouble deltaTime = 0.0f;
GLdouble lastFrame = 0.0f;

// Scene
Scene *scene;
Physics physicSystem;

// Shaders and rendering 
ParticleRenderer *particleRenderer;

// Light
const vec3 lightPosition = vec3(50.0f);

// Simulation variables and parameters
bool doPyshics = false;
bool showPerformance = false;
bool showModels = false;


static void errorCallback(int error, const char* description) {
	std::cerr << "Error: " << description << std::endl;
}

void init() {
    glfwSetErrorCallback(errorCallback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Simple example", NULL, NULL);
#if _DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!window) {
        std::cout << "Failed to crate GLFW window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

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

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

    ImGui_ImplGlfwGL3_Init(window, true); 

	input::initialize(window);

    visualization::initialize();

	performance::initialize();

	scene = new Scene;
    physicSystem = Physics();

    physicSystem.iterations = 5;
    physicSystem.pSleeping = 0.0001f;
    physicSystem.overRelaxConst = 1.0f;
    physicSystem.restitutionCoefficient = 1.f; // 1 is Elastic collision


    scene->init();

	model::loadModelNames();

	// Load some model to begin with, so that debugging is easier on us
	model::modelConfig conf;
	conf.centerPos = vec3(0.f);
	conf.distanceThreshold = 2;
	conf.invmass = 0.8f;
	conf.phase = 0;
	conf.scale = vec3(4.f);
	conf.stiffness = 0.0001f;
	conf.numParticles = ivec3(4);
	model::loadPredefinedModel("Box", physicSystem.particles, physicSystem.constraints, conf);

	particleRenderer = new ParticleRenderer();
	particleRenderer->init();
}

void display() {

	int id = performance::startTimer("Reset and draw scene");

    float ratio;
    int width, height;
    mat4 viewMatrix, modelViewProjectionMatrix, modelViewMatrix, projectionMatrix;

    glfwGetFramebufferSize(window, &width, &height);
    ratio = (GLfloat)WIDTH / (GLfloat)HEIGHT;

    glViewport(0, 0, width, height);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 modelMatrix(1.0f); // Identity matrix

	viewMatrix = camera.getViewMatrix();
    // Set up a projection matrix
    float nearPlane = 0.01f;
    float farPlane = 1000.0f;

    modelViewMatrix = viewMatrix * modelMatrix;
	projectionMatrix = perspective(radians(camera.getFovy()), ratio, nearPlane, farPlane);
    modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

    vec3 viewSpaceLightPosition = vec3(viewMatrix * vec4(lightPosition, 1.0));
	
    scene->render(viewMatrix, projectionMatrix);
    
	performance::stopTimer(id);
	
    if (doPyshics)
    {
        physicSystem.step(scene, ImGui::GetIO().DeltaTime);
    }
	
    id = performance::startTimer("Render particles");
    particleRenderer->render(physicSystem.particles, modelViewProjectionMatrix, modelViewMatrix, viewSpaceLightPosition, projectionMatrix);
	performance::stopTimer(id);

    visualization::drawConstraints(physicSystem.constraints, physicSystem.particles, modelViewProjectionMatrix);

	// Since we may want to measure performance of something that happens after the call to gui()
	// we place this call as late as possible to allow for measuring more things
	performance::gui(&showPerformance);

    ImGui::Render();

    glfwSwapBuffers(window);
}

void gui()
{
    // Consider scapping incase of performance
    glfwSwapInterval(vsync ? 1 : 0);

    static bool show_demo_window = false;

    ImGui::Begin("IMPEngine");
    ImGui::ColorEdit3("clear color", (float*)&clear_color);
    ImGui::Checkbox("Vsync", &vsync);
    if (ImGui::Button("Demo Window")) show_demo_window ^= 1;
	if (ImGui::Button("Models")) showModels ^= 1; ImGui::SameLine();
	if (ImGui::Button("Performance Window CPU")) showPerformance ^= 1;
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	visualization::gui();
    ImGui::Checkbox("Physics", &doPyshics);
    ImGui::SliderInt("Solver Iterations", &physicSystem.iterations, 1, 32);
    ImGui::SliderFloat("Over-relax-constant", &physicSystem.overRelaxConst, 1, 5);
    ImGui::SliderFloat("Particle Sleeping (squared)", &physicSystem.pSleeping, 0, 1, "%.9f", 10.f);
	ImGui::SliderFloat("Restitution Coeff.", &physicSystem.restitutionCoefficient, 0, 1);
	ImGui::End();

	model::gui(&showModels, physicSystem.particles, physicSystem.constraints);

    // Remove when all group members feel comfortable with how GUI works and what it can provide
    // Demo window
    if (show_demo_window)
    {
        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
        ImGui::ShowTestWindow(&show_demo_window);
    }
}

int main(void) {

#ifdef _DEBUG

    doIntersectionTests();

#endif // _DEBUG

    init();

    double startTime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
		int id = performance::startTimer("All but display()");

        // Calculate deltatime of current frame
        GLdouble currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
		camera.move(input::getKeys(), (float)deltaTime);

        ImGui_ImplGlfwGL3_NewFrame();

        gui();

		performance::stopTimer(id);

        display();
		performance::next();
    }

    // Cleanup
    glfwDestroyWindow(window);
    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

