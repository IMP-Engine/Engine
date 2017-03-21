#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

using namespace glm;

class Camera
{
public:
	
	Camera(vec3 cameraPos = vec3(0.0f, 0.0f, 30.0f), vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f),
		vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f), GLfloat yaw = -90.f, GLfloat pitch = 0.f, GLfloat fovy = 45.f);

	mat4 getViewMatrix();

	void mouseMovement(GLfloat newx, GLfloat newy);

	void mouseButton(GLfloat newx, GLfloat newy);

	void mouseScroll(GLfloat yoffset);

	void move(bool keys[], float deltaTime);

	GLfloat getFovy() { return this->fovy; }

private:
	vec3 cameraPos;
	vec3 cameraFront;
	vec3 cameraUp;
	GLfloat yaw;
	GLfloat pitch;
	GLfloat lastX = 0;
	GLfloat lastY = 0;
	GLfloat fovy;

	GLfloat xoffset, yoffset; // not necessarily global if camera movement slide doesn't need it
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