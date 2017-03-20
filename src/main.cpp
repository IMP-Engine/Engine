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

#include "constraints/DistanceConstraint.h"
#include "constraints/visualizeConstraint.h"
#include "models/model.h"

#ifdef _DEBUG
#include "intersections/tests.h"
#endif // _DEB

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))


#define MAX_FOV 70.0f
#define MIN_FOV 1.0f

using namespace glm;
using namespace std;

/*************************************************************************
 ********************** Global variables **********************************
 **************************************************************************/

// Global variables
#define MAX_FOV 70.0f
#define MIN_FOV 1.0f


// Application
GLFWwindow* window;
ImVec4 clear_color = ImColor(255, 255, 255);;
bool vsync = true;
const GLuint WIDTH = 1280, HEIGHT = 720;


// Camera
vec3 cameraPos = vec3(0.0f, 0.0f, 30.0f);
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);
GLfloat yaw = -90.0f;
GLfloat pitch = 0.0f;
GLfloat lastX = WIDTH / 2;
GLfloat lastY = HEIGHT / 2;
GLfloat fovy = 70.0f;
bool keys[1024];
GLfloat xoffset, yoffset; // not necessarily global if camera movement slide doesn't need it
#define slideCoefficient 10 // lower = longer slide
vec3 forwardMovement = vec3(0.0f);
vec3 backwardMovement = vec3(0.0f);
vec3 rightMovement = vec3(0.0f);
vec3 leftMovement = vec3(0.0f);
vec3 upMovement = vec3(0.0f);
vec3 downMovement = vec3(0.0f);

// Deltatime
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// Scene
Scene *scene;
vector<Particle> particles;
vector<Constraint *> constraints;

// Shaders and rendering 
ParticleRenderer *particleRenderer;

// Light
const vec3 lightPosition = vec3(50.0f);


// Simulation variables and parameters
bool doPyshics = false;
int iterations = 5;
bool showPerformance = false;
bool showModels = false;
float pSleeping = 0.0001f;
float overRelaxConst = 1.0f;
float restitutionCoefficient = 1.f; // 1 is Elastic collision

std::vector<Particle> particles;
std::vector<Constraint*> constraints;


static void errorCallback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        xoffset = (GLfloat)xpos - lastX;
        yoffset = lastY - (GLfloat)ypos;
        lastX = (GLfloat)xpos;
        lastY = (GLfloat)ypos;

        GLfloat sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw = mod(yaw + xoffset, (GLfloat)360.0f);
        pitch += yoffset;

        // Limit pitch within (-90, 90) degrees
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        vec3 front;
        front.x = cos(radians(pitch)) * cos(radians(yaw));
        front.y = sin(radians(pitch));
        front.z = cos(radians(pitch)) * sin(radians(yaw));
        cameraFront = normalize(front);
    }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        cout << "Press." << endl;
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        lastX = (GLfloat)x;
        lastY = (GLfloat)y;
    }
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    if (fovy >= MIN_FOV && fovy <= MAX_FOV)
        fovy -= (GLfloat)yoffset;
    else if (fovy <= MIN_FOV)
        fovy = MIN_FOV;
    else 
        fovy = MAX_FOV;
}

void doMovement() {
    GLfloat cameraSpeed = 10.0f * deltaTime;
    if (keys[GLFW_KEY_W])
        forwardMovement = cameraSpeed * cameraFront;
    if (keys[GLFW_KEY_S]) 
        backwardMovement = - cameraSpeed * cameraFront;
    if (keys[GLFW_KEY_A])
        leftMovement = - normalize(cross(cameraFront, cameraUp)) * cameraSpeed;
    if (keys[GLFW_KEY_D])
        rightMovement = normalize(cross(cameraFront, cameraUp)) * cameraSpeed;
    if (keys[GLFW_KEY_SPACE])
        upMovement = cameraSpeed * cameraUp;
    if (keys[GLFW_KEY_LEFT_CONTROL])
        downMovement = - cameraSpeed * cameraUp;
    cameraPos += forwardMovement;
    cameraPos += backwardMovement;
    cameraPos += rightMovement;
    cameraPos += leftMovement;
    cameraPos += upMovement;
    cameraPos += downMovement;
    // Camera slide after input has finished
    forwardMovement *= (1.0f - deltaTime * slideCoefficient);
    backwardMovement *= (1.0f - deltaTime * slideCoefficient);
    rightMovement *= (1.0f - deltaTime * slideCoefficient);
    leftMovement *= (1.0f - deltaTime * slideCoefficient);
    upMovement *= (1.0f - deltaTime * slideCoefficient);
    downMovement *= (1.0f - deltaTime * slideCoefficient);
}

void initGL() {
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

    ImGui_ImplGlfwGL3_Init(window, true); 

    // steal callback and call imgui in our callback
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);    
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    // Constraint visualization setup
    visualization::initialize();

    scene->init();
   

    glEnable(GL_POINT_SPRITE);
    //glPointSize(10.1f);
    //glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT); // Not sure if needed, keeping meanwhile

    // Not sure which one to use, keeping both meanwhile
    glEnable(GL_PROGRAM_POINT_SIZE);
    //glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);


}

void display() {

	int id = performance::startTimer("Reset and draw scene");

    float ratio;
    int width, height;
    mat4 viewMatrix, modelViewProjectionMatrix, modelViewMatrix, projectionMatrix;

    glfwGetFramebufferSize(window, &width, &height);
    //ratio = width / (float)height;
    ratio = (GLfloat)WIDTH / (GLfloat)HEIGHT;

    glViewport(0, 0, width, height);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 modelMatrix(1.0f); // Identity matrix

    viewMatrix = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    // Set up a projection matrix
    float fovy = radians(45.0f);
    float nearPlane = 0.01f;
    float farPlane = 300.0f;

    modelViewMatrix = viewMatrix * modelMatrix;
    projectionMatrix = perspective(fovy, ratio, nearPlane, farPlane);
    modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

    vec3 viewSpaceLightPosition = vec3(viewMatrix * vec4(lightPosition, 1.0));
	
    scene->render(viewMatrix, projectionMatrix);
    
	performance::stopTimer(id);
	
    if (doPyshics)
    {
		physics::simulate(particles, constraints, scene, ImGui::GetIO().DeltaTime, iterations);
    }
	
    id = performance::startTimer("Render particles");
    particleRenderer->render(modelViewProjectionMatrix, modelViewMatrix, viewSpaceLightPosition, projectionMatrix);
	performance::stopTimer(id);

    visualization::drawConstraints(&constraints, modelViewProjectionMatrix);



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
	if (ImGui::Button("Models")) showModels ^= 1; ImGui::SameLine();
	if (ImGui::Button("Performance Window CPU")) showPerformance ^= 1;
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::PlotLines("", frameTimes, COUNT_OF(frameTimes), offset, "Time/Frame [s]", FLT_MIN, FLT_MAX, ImVec2(0, 80));
	visualization::gui();
    ImGui::Checkbox("Physics", &doPyshics);
    ImGui::SliderInt("Solver Iterations", &iterations, 1, 32);
    ImGui::SliderFloat("Over-relax-constant", &overRelaxConst, 1, 5);
    ImGui::SliderFloat("Particle Sleeping (squared)", &pSleeping, 0, 1, "%.9f", 10.f);
	ImGui::SliderFloat("Restitution Coeff.", &restitutionCoefficient, 0, 1);
	ImGui::End();

	model::gui(&showModels);

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


    scene = new Scene;
    particles.reserve(200000);
	constraints.reserve(2000000);

    performance::initialize();

    initGL();
	model::loadModelNames();

	// Test actually creating something from the repo
	tbb::empty_task a();

	// Load some model to begin with, so that debugging is easier on us
	model::modelConfig conf;
	conf.centerPos = vec3(0.f);
	conf.distanceThreshold = 2;
	conf.invmass = 0.1;
	conf.phase = 0;
	conf.scale = vec3(4.f);
	conf.stiffness = 0.8f;
	conf.numParticles = ivec3(4);
	model::loadPredefinedModel("Box", &particles, &constraints, conf);

	particleRenderer = new ParticleRenderer(&particles);
	particleRenderer->init();

    if (GLAD_GL_VERSION_4_3) {
        /* We support at least OpenGL version 4.3 */
    }

    double startTime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
		int id = performance::startTimer("All but display()");

        // Calculate deltatime of current frame
        GLfloat currentFrame = (GLfloat)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        doMovement();

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

