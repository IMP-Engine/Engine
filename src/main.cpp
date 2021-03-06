#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"
#include <iostream>
#include "debug.h"

#include "performance.h"
#include "camera.h"

#include "glHelper.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>


#define WORLD_MIN vec3(-20.f,-20.f,-20.f)
#define WORLD_MAX vec3( 20.f, 20.f, 20.f)

#include "collision/collision.h"
#include "performance.h"
#include "physics.h"
#include "particles/ParticleRenderer.h"
#include "scenes/Scene.h"
#include "models/ModelRenderer.h"

#include "constraints/visualizeConstraint.h"
#include "models/modelConfig.h"
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
ImVec4 clear_color = ImColor(30, 30, 30);;
bool vsync = true;
const GLuint WIDTH = 1920, HEIGHT = 1080;

// Camera
Camera camera;

// Deltatime
GLdouble deltaTime = 0.0f;
GLdouble lastFrame = 0.0f;

int bPhysics;
int aPhysics;

// Scene
Scene *scene;
Physics physicSystem;
std::vector< std::tuple<std::string, model::ModelConfig> > objects;

// Shaders and rendering 
ParticleRenderer *particleRenderer;
ModelRenderer *modelRenderer;
bool renderSurfaces = false;

// Models
ModelData modelData;

// Light
const vec3 lightPosition = vec3(4.0f);

// Simulation variables and parameters
bool doPyshics = false;
bool showModels = false;
bool showCollision = false;
bool useVariableTimestep = false;
float timestep = 0.01667f;
bool showSceneSelection = false;
bool showPerformance = false;

bool boxSpawned = false;
bool boxhold = false;


static void errorCallback(int error, const char* description) {
    std::cerr << "Error: " << description << std::endl;
}

void init() {
    glfwSetErrorCallback(errorCallback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(WIDTH, HEIGHT, "IMP Engine", NULL, NULL);
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
    camera.physicSystem = &physicSystem;

    visualization::initialize();

    performance::initialize();

    scene = new Scene;
    physicSystem = Physics();

    physicSystem.iterations = 4;
    physicSystem.stabilizationIterations = 2;
    physicSystem.pSleeping = 0.0001f;
    physicSystem.overRelaxConst = 1.0f;
    physicSystem.restitutionCoefficientT = 0.8f;
    physicSystem.restitutionCoefficientN = 0.8f;
    physicSystem.parallelConstraintSolve = false;
    physicSystem.parallelDetectCollisions = false;
    physicSystem.kineticFC = 0.2f;
    physicSystem.staticFC = 0.2f;

    modelData = ModelData();
    modelData.clear();

    scene->init();

    model::loadModelNames();

    // Load some model to begin with, so that debugging is easier on us
    model::ModelConfig conf;
    conf.setDefaults();
    model::loadPredefinedModel("Box", physicSystem.particles, physicSystem.constraints, conf);

    particleRenderer = new ParticleRenderer();
    particleRenderer->init();

    modelRenderer = new ModelRenderer();
    modelRenderer->init();
}

void display(double deltaTime) {
    GLfloat ratio;
    GLint width, height;
    mat4 viewMatrix, modelViewProjectionMatrix, modelViewMatrix, projectionMatrix;

    glfwGetFramebufferSize(window, &width, &height);
    ratio = (GLfloat)width / (GLfloat)height;

    glViewport(0, 0, width, height);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    viewMatrix = camera.getViewMatrix();
    // Set up a projection matrix
    float nearPlane = 0.01f;
    float farPlane = 1000.0f;

    // Model matrix is identity
    modelViewMatrix = viewMatrix;
    projectionMatrix = perspective(radians(camera.getFovy()), ratio, nearPlane, farPlane);
    modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

    vec3 viewSpaceLightPosition = vec3(viewMatrix * vec4(lightPosition, 1.0));

    scene->render(viewMatrix, modelViewProjectionMatrix, vec3(lightPosition));

    performance::stopTimer(bPhysics);

    if (doPyshics)
    {
        physicSystem.step(scene, useVariableTimestep ? (float)deltaTime : timestep);
    }

    aPhysics = performance::startTimer("After physics");

    if (renderSurfaces)
    {
        int id = performance::startTimer("Render surfaces");
        modelRenderer->render(physicSystem.particles, modelData, modelViewProjectionMatrix, modelViewMatrix, lightPosition, projectionMatrix);

    }
    else // render particles
    {
        int viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);

        GLint heightOfNearPlane = (GLint)round(abs(viewport[3] - viewport[1]) / (2 * tan(0.5*camera.getFovy() * glm::pi<float>() / 180.0)));

        particleRenderer->render(physicSystem.particles, modelViewProjectionMatrix, modelViewMatrix, viewSpaceLightPosition, projectionMatrix, heightOfNearPlane);

        visualization::drawConstraints(physicSystem.constraints, physicSystem.particles, modelViewProjectionMatrix);
    }

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
    if (ImGui::Button("Scenes")) showSceneSelection ^= 1; ImGui::SameLine();
    if (ImGui::Button("Performance Window CPU")) showPerformance ^= 1; ImGui::SameLine();
    if (ImGui::Button("Collision detection")) showCollision ^= 1;
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    visualization::gui();
    ImGui::Checkbox("Physics", &doPyshics); ImGui::SameLine();
    ImGui::Checkbox("Timestep from framerate", &useVariableTimestep);
    ImGui::Checkbox("Parallel constraint solve", &physicSystem.parallelConstraintSolve); ImGui::SameLine();
    //ImGui::Checkbox("Parallel collision detection", &physicSystem.parallelDetectCollisions);
    ImGui::Checkbox("Apply windlike force", &scene->windActive);
    ImGui::Checkbox("Render surfaces", &renderSurfaces);
    ImGui::SliderInt("Solver Iterations", &physicSystem.iterations, 1, 32);
    ImGui::SliderInt("Collision Stabilization Iterations", &physicSystem.stabilizationIterations, 0, 32);
    ImGui::SliderFloat("Over-relax-constant", &physicSystem.overRelaxConst, 1, 5);
    ImGui::SliderFloat("Particle Sleeping (squared)", &physicSystem.pSleeping, 0, 1, "%.9f", 10.f);
    ImGui::SliderFloat("Tangential COR", &physicSystem.restitutionCoefficientT, -1, 1);
    ImGui::SliderFloat("Normal COR", &physicSystem.restitutionCoefficientN, 0, 1);
    ImGui::SliderFloat("Kinetic Friction Coefficient", &physicSystem.kineticFC, 0, 1);
    ImGui::SliderFloat("Static Friction Coefficient", &physicSystem.staticFC, 0, 1);
    if (!useVariableTimestep) 
    {
        ImGui::SliderFloat("Timestep", &timestep, 0, .05f, "%.5f"); ImGui::SameLine();
        ImGui::Text((std::to_string(1 / timestep) + " \"FPS\"").c_str());
    }
    ImGui::End();

    collision::gui(&showCollision);
    model::gui(&showModels, physicSystem.particles, physicSystem.constraints, objects, modelData);
    scene->gui(&showSceneSelection);
}

int main(void) {

#ifdef _DEBUG

    doIntersectionTests();

#endif // _DEBUG

    init();

    double startTime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        bPhysics = performance::startTimer("Before physics");
        // Calculate deltatime of current frame
        GLdouble currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        camera.move(input::getKeys(), (float)deltaTime);

        ImGui_ImplGlfwGL3_NewFrame();

        gui();

        display(deltaTime);
        performance::stopTimer(aPhysics);
        performance::next();
    }

    // Cleanup
    glfwDestroyWindow(window);
    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

