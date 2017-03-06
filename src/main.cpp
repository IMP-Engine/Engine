#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"
#include <iostream>
#include "debug.h"

#include "glHelper.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>


#define WORLD_MIN vec3(-20.f,-20.f,-20.f)
#define WORLD_MAX vec3( 20.f, 20.f, 20.f)

#include "physics.h"
#include "particles/ParticleRenderer.h"
#include "particles/Box.h"
#include "Scene.h"
#include "DistanceConstraint.h"

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

// Global variables
#define MAX_FOV 70.0f
#define MIN_FOV 1.0f


// Application
GLFWwindow* window;
ImVec4 clear_color = ImColor(164, 164, 164);;
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

// Box parameters
Box *box1, *box2;
ivec3 numparticles = ivec3(3, 3, 3);
vec3 dimension = vec3(1.f, 1.f, 1.f);
float mass = 30.f;
float stiffness = 0.5f;

// Scene
Scene *scene;
vector<Particle> particles;

// Shaders and rendering 
GLuint simpleShader;
ParticleRenderer *particleRenderer;

// Light
const vec3 lightPosition = vec3(50.0f);


// Simulation variables and parameters
bool doPyshics = false;
int iterations = 5;


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


    // Shader setup
    simpleShader = glHelper::loadShader(VERT_SHADER_PATH, FRAG_SHADER_PATH);
    GLuint mvp_location = glGetUniformLocation(simpleShader, "MVP");
    GLuint vpos_location = glGetAttribLocation(simpleShader, "vPos");

	// Scene setup
	glGenVertexArrays(1, &scene->vao);
	glBindVertexArray(scene->vao);

	glGenBuffers(1, &scene->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, scene->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * scene->vertexes.size(), &(scene->vertexes[0]), GL_STATIC_DRAW);

    glGenBuffers(1, &scene->ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, scene->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLshort) * scene->indices.size(), &(scene->indices[0]), GL_STATIC_DRAW);

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    

    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

	glEnable(GL_POINT_SPRITE);
    glPointSize(0.1f);
	//glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT); // Not sure if needed, keeping meanwhile

	// Not sure which one to use, keeping both meanwhile
	glEnable(GL_PROGRAM_POINT_SIZE);
	//glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

	//particleRenderer->init();

}

/*
* Creates a box with given parameters and hooks it up to rendering. Also makes sure that any old box is removed.
*/
void setupBox(vec3 dimension, vec3 centerpos, float totmass, ivec3 numParticles, float stiffness)
{
    delete box1;
    delete box2;
    particles.clear();

	delete particleRenderer;
	BoxConfig config;

	config.dimensions = dimension;
	config.center_pos = centerpos;
	config.mass = totmass;
	config.phase = 1;
	config.numParticles = numParticles;
	config.stiffness = stiffness;
	box1 = make_box(&config, particles);
    
    //config.center_pos += vec3(0.55f, 1.55f, 0.55f);
    //config.phase = 2;

    //box2 = make_box(&config, particles);



    //particles.insert(particles.end(), box1->particles.begin(), box1->particles.end());
    //particles.insert(particles.end(), box2->particles.begin(), box2->particles.end());


    particleRenderer = new ParticleRenderer(&particles);
    particleRenderer->init();
}

void display() {
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

    //modelMatrix = translate(modelMatrix, vec3(0.0f, 0.0f, 0.0f));
    //modelMatrix = scale(modelMatrix, vec3(5.0f, 5.0f, 5.0f));


    // Send uniforms to shader
    glUseProgram(simpleShader);
    glUniformMatrix4fv(glGetUniformLocation(simpleShader, "modelViewProjectionMatrix"), 1, false, &modelViewProjectionMatrix[0].x);
    glUniformMatrix4fv(glGetUniformLocation(simpleShader, "modelViewMatrix"), 1, false, &modelViewMatrix[0].x);

    // Draw cube
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBindVertexArray(scene->vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, scene->ibo);
    int size;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0); //sizeof(GLushort),

    // GUI
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	if(doPyshics)
		physics::simulate(particles, box1->constraints, scene, ImGui::GetIO().DeltaTime, iterations);
	particleRenderer->render(modelViewProjectionMatrix, modelViewMatrix, viewSpaceLightPosition, projectionMatrix);

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
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::PlotLines("", frameTimes, COUNT_OF(frameTimes), offset, "Time/Frame [s]", FLT_MIN, FLT_MAX, ImVec2(0, 80));
	ImGui::Checkbox("Physics", &doPyshics);
	ImGui::SliderInt("Solver Iterations", &iterations, 1, 32);
	ImGui::SliderInt("Particles x", &numparticles.x, 1, 10);
	ImGui::SliderInt("Particles y", &numparticles.y, 1, 10);
	ImGui::SliderInt("Particles z", &numparticles.z, 1, 10);
	ImGui::SliderFloat("Dimension x", &dimension.x, 0, 10);
	ImGui::SliderFloat("Dimension y", &dimension.y, 0, 10);
	ImGui::SliderFloat("Dimension z", &dimension.z, 0, 10);
	ImGui::SliderFloat("Stiffness", &stiffness, 0, 1);
	ImGui::SliderFloat("Mass (averaged over particles)", &mass, 0, 10000, "%.3f", 10.f);
	if (ImGui::Button("reset"))
		setupBox(dimension, vec3(0.f, 0.f, 0.f), mass, numparticles, stiffness);
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

    scene = new Scene;
    particles.resize(100000);
    initGL();

    setupBox(vec3(1.f, 1.f, 1.f), vec3(0.f, 0.f, 0.f), 125.f, ivec3(5, 5, 5), stiffness);

	if (GLAD_GL_VERSION_4_3) {
		/* We support at least OpenGL version 4.3 */
	}

	double startTime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        // Calculate deltatime of current frame
        GLfloat currentFrame = (GLfloat)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        doMovement();

        ImGui_ImplGlfwGL3_NewFrame();

        gui();

        display();
    }

    // Cleanup
    glfwDestroyWindow(window);
    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

