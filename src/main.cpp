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

#include "physics.h"
#include "particles/ParticleRenderer.h"
#include "particles/Box.h"
#include "constraints/DistanceConstraint.h"

#include "constraints/visualizeConstraint.h"

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

#ifdef _WIN32
#define VERT_SHADER_PATH "../../src/shaders/simple.vert"
#define FRAG_SHADER_PATH "../../src/shaders/simple.frag"
#elif __unix__
#define VERT_SHADER_PATH "../src/shaders/simple.vert"
#define FRAG_SHADER_PATH "../src/shaders/simple.frag"
#endif

#define MAX_FOV 70.0f
#define MIN_FOV 1.0f

using namespace glm;
using namespace std;

/*************************************************************************
 ********************** Global variables **********************************
 **************************************************************************/

// Application
GLFWwindow* window;
ImVec4 clear_color = ImColor(255, 255, 255);;
bool vsync = true;
const GLuint WIDTH = 1280, HEIGHT = 720;

GLint mvp_location, vpos_location, vcol_location;
GLuint cube_ibo; // IndicesBufferObject

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

// Shaders and rendering 
GLuint simpleShader, particleShader;
ParticleRenderer *particleRenderer;

// Light
const vec3 lightPosition = vec3(50.0f);

// VAOs
GLuint simpleVao, particleVao;

// Simulation variables and parameters
bool doPyshics = false;
int iterations = 5;
bool showPerformance = false;
float pSleeping = 0.001;
float overRelaxConst = 1.0f;


// Box parameters
Box *box;
ivec3 numparticles = vec3(5, 5, 5);
vec3 dimension = vec3(1.f, 1.f, 1.f);
float mass = 30.f;
float stiffness = 0.5f;
float distanceThreshold = 0.2f;

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

/*
   float cubeVertices[] = {
   -0.5f, -0.5f,  0.5f,
   0.5f, -0.5f,  0.5f,
   0.5f,  0.5f,  0.5f,
   -0.5f,  0.5f,  0.5f,

   -0.5f, -0.5f, -0.5f,
   0.5f, -0.5f, -0.5f,
   0.5f,  0.5f, -0.5f,
   -0.5f,  0.5f, -0.5f,
   };
   */

vec3 cubePositions[] = {
    vec3(-3.0f, 0.0f, 0.0f),
    vec3(3.0f, 0.0f, 0.0f)
};

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
        xoffset = xpos - lastX;
        yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;

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
        lastX = x;
        lastY = y;
    }
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    if (fovy >= MIN_FOV && fovy <= MAX_FOV)
        fovy -= yoffset;
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

    // Shader setup
    simpleShader = glHelper::loadShader(VERT_SHADER_PATH, FRAG_SHADER_PATH);
    mvp_location = glGetUniformLocation(simpleShader, "MVP");
    vpos_location = glGetAttribLocation(simpleShader, "vPos");
    vcol_location = glGetAttribLocation(simpleShader, "vCol");

    // Triangle setup
    GLuint vertex_buffer;

    glGenVertexArrays(1, &simpleVao);
    glBindVertexArray(simpleVao);

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

	glEnable(GL_POINT_SPRITE);
  //glPointSize(10.0f);
	//glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT); // Not sure if needed, keeping meanwhile


    // Not sure which one to use, keeping both meanwhile
    glEnable(GL_PROGRAM_POINT_SIZE);
    //glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

    //particleRenderer->init();

}

/*
 * Creates a box with given parameters and hooks it up to rendering. Also makes sure that any old box is removed.
 */
void setupBox(vec3 dimension, vec3 centerpos, float totmass, vec3 numparticles, float stiffness, float distanceThreshold)
{
    delete box;
    delete particleRenderer;
    BoxConfig config;

    config.dimensions = dimension;
    config.center_pos = centerpos;
    config.mass = totmass;
    config.phase = 1;
    config.num_particles = numparticles;
    config.stiffness = stiffness;
    config.distanceThreshold = distanceThreshold;
    box = make_box(&config);

    std::cout << "Constraints: " << box->constraints.size() << std::endl;

    particleRenderer = new ParticleRenderer(&box->particles);
    particleRenderer->init();
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

    //vec3 cameraTarget = vec3(0.0f, 0.0f, 0.0f);
    //vec3 cameraDirection = normalize(cameraPos - cameraTarget);
    //vec3 cameraRight = normalize(cross(cameraUp, cameraDirection));

    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float)height;

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

    //modelMatrix = translate(modelMatrix, vec3(0.0f, 0.0f, 0.0f));
    //modelMatrix = scale(modelMatrix, vec3(5.0f, 5.0f, 5.0f));


    // Send uniforms to shader
    glUseProgram(simpleShader);
    glUniformMatrix4fv(glGetUniformLocation(simpleShader, "modelViewProjectionMatrix"), 1, false, &modelViewProjectionMatrix[0].x);
    glUniformMatrix4fv(glGetUniformLocation(simpleShader, "modelViewMatrix"), 1, false, &modelViewMatrix[0].x);

    // Draw cube
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glBindVertexArray(simpleVao);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ibo);

	  performance::stopTimer(id);

    int size;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0); //sizeof(GLushort),
    glBindVertexArray(0);

    // GUI
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	  if(doPyshics)
		    physics::simulate(&box->particles, &box->constraints, ImGui::GetIO().DeltaTime, iterations);
	
    visualization::drawConstraints(&box->constraints, modelViewProjectionMatrix);
  
	  id = performance::startTimer("Render particles");
	  particleRenderer->render(modelViewProjectionMatrix, modelViewMatrix, viewSpaceLightPosition, projectionMatrix);
	  performance::stopTimer(id);


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
	if (ImGui::Button("Performance Window CPU")) showPerformance ^= 1;
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::PlotLines("", frameTimes, COUNT_OF(frameTimes), offset, "Time/Frame [s]", FLT_MIN, FLT_MAX, ImVec2(0, 80));
	visualization::gui();
    ImGui::Checkbox("Physics", &doPyshics);
    ImGui::SliderInt("Solver Iterations", &iterations, 1, 32);
    ImGui::SliderFloat("Over-relax-constant", &overRelaxConst, 1, 5);
    ImGui::SliderFloat("Particle Sleeping (squared)", &pSleeping, 0, 1, "%.9f", 10.f);
    ImGui::SliderInt("Particles x", &numparticles.x, 1, 10);
    ImGui::SliderInt("Particles y", &numparticles.y, 1, 10);
    ImGui::SliderInt("Particles z", &numparticles.z, 1, 10);
    ImGui::SliderFloat("Dimension x", &dimension.x, 0, 10);
    ImGui::SliderFloat("Dimension y", &dimension.y, 0, 10);
    ImGui::SliderFloat("Dimension z", &dimension.z, 0, 10);
    ImGui::SliderFloat("Stiffness", &stiffness, 0, 1);
    ImGui::SliderFloat("Mass (averaged over particles)", &mass, 0, 10000, "%.3f", 10.f);
    ImGui::SliderFloat("Distance threshold", &distanceThreshold, 0, 1);
    if (ImGui::Button("reset"))
    {
        setupBox(dimension, vec3(0.f, 0.f, 0.f), mass, numparticles, stiffness, distanceThreshold);
    }
    ImGui::End();


    // Remove when all group members feel comfortable with how GUI works and what it can provide
    // Demo window
    if (show_demo_window)
    {
        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
        ImGui::ShowTestWindow(&show_demo_window);
    }
}

int main(void) {
	  performance::initialize();
    initGL();
    setupBox(vec3(1.f, 1.f, 1.f), vec3(0.f, 0.f, 0.f), 125.f, vec3(5, 5, 5), stiffness, distanceThreshold);

	// Test actually creating something from the repo
	tbb::empty_task a();

    if (GLAD_GL_VERSION_4_3) {
        /* We support at least OpenGL version 4.3 */
    }

    double startTime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
		int id = performance::startTimer("All but display()");

        // Calculate deltatime of current frame
        GLfloat currentFrame = glfwGetTime();
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

