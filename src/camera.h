#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include "input.h"
#include "physics.h"
#include "models/Box.h"
#include "models/modelConfig.h"

using namespace glm;

class Camera : input::Observer
{
public:
	
	Camera(vec3 cameraPos = vec3(0.0f, 0.0f, 30.0f), vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f),
		vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f), GLfloat yaw = -90.f, GLfloat pitch = 0.f, GLfloat fovy = 45.f);

	mat4 getViewMatrix();

	void mouseCallback(GLFWwindow* window, double newx, double newy);

	void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

	void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

	void keyCallback(GLFWwindow*, int, int, int, int) {}

	void move(bool keys[], float deltaTime);

	float getFovy() { return this->fovy; }

    Physics *physicSystem;

private:
	vec3 cameraPos;
	vec3 cameraFront;
	vec3 cameraUp;
	float yaw;
	float pitch;
	float lastX = 0;
	float lastY = 0;
	float fovy;
	
	float xoffset, yoffset; // not necessarily global if camera movement slide doesn't need it
#define slideCoefficient 10 // lower = longer slide
#define MAX_FOV 70.0f
#define MIN_FOV 1.0f
	vec3 forwardMovement = vec3(0.0f);
	vec3 backwardMovement = vec3(0.0f);
	vec3 rightMovement = vec3(0.0f);
	vec3 leftMovement = vec3(0.0f);
	vec3 upMovement = vec3(0.0f);
	vec3 downMovement = vec3(0.0f);

};