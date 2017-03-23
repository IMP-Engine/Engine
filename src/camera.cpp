#include "camera.h"

Camera::Camera(vec3 cameraPos, vec3 cameraUp, vec3 cameraFront, GLfloat yaw, GLfloat pitch, GLfloat fovy)
{
	this->cameraPos = cameraPos;
	this->cameraUp = cameraUp;
	this->cameraFront = cameraFront;
	this->yaw = yaw;
	this->pitch = pitch;
	this->fovy = fovy;
	input::subscribeObserver(this);
}

mat4 Camera::getViewMatrix()
{
	return lookAt(this->cameraPos, this->cameraPos + this->cameraFront, this->cameraUp);
}

void Camera::mouseCallback(GLFWwindow* window, double newx, double newy)
{

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS) {
		return;
	}

	this->xoffset = newx - this->lastX;
	this->yoffset = this->lastY - newy;
	this->lastX = newx;
	this->lastY = newy;

	GLfloat sensitivity = 0.1f;
	this->xoffset *= sensitivity;
	this->yoffset *= sensitivity;

	this->yaw = mod(this->yaw + this->xoffset, (GLfloat)360.0f);
	this->pitch += this->yoffset;

	// Limit pitch within (-90, 90) degrees
	if (this->pitch > 89.0f)
		this->pitch = 89.0f;
	if (this->pitch < -89.0f)
		this->pitch = -89.0f;

	vec3 front;
	front.x = cos(radians(pitch)) * cos(radians(yaw));
	front.y = sin(radians(pitch));
	front.z = cos(radians(pitch)) * sin(radians(yaw));
	this->cameraFront = normalize(front);
}

void Camera::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		this->lastX = x;
		this->lastY = y;
	}
	
}

void Camera::mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (this->fovy >= MIN_FOV && this->fovy <= MAX_FOV)
		this->fovy -= yoffset;
	else if (this->fovy <= MIN_FOV)
		this->fovy = MIN_FOV;
	else
		this->fovy = MAX_FOV;
}

void Camera::move(bool keys[], float deltaTime)
{
	GLfloat cameraSpeed = 10.0f * deltaTime;
	if (keys[GLFW_KEY_W])
		this->forwardMovement = cameraSpeed * this->cameraFront;
	if (keys[GLFW_KEY_S])
		this->backwardMovement = -cameraSpeed * this->cameraFront;
	if (keys[GLFW_KEY_A])
		this->leftMovement = -normalize(cross(this->cameraFront, this->cameraUp)) * cameraSpeed;
	if (keys[GLFW_KEY_D])
		this->rightMovement = normalize(cross(this->cameraFront, this->cameraUp)) * cameraSpeed;
	if (keys[GLFW_KEY_SPACE])
		this->upMovement = cameraSpeed * this->cameraUp;
	if (keys[GLFW_KEY_LEFT_CONTROL])
		this->downMovement = -cameraSpeed * this->cameraUp;
	this->cameraPos += this->forwardMovement;
	this->cameraPos += this->backwardMovement;
	this->cameraPos += this->rightMovement;
	this->cameraPos += this->leftMovement;
	this->cameraPos += this->upMovement;
	this->cameraPos += this->downMovement;
	// Camera slide after input has finished
	this->forwardMovement *= (1.0f - deltaTime * slideCoefficient);
	this->backwardMovement *= (1.0f - deltaTime * slideCoefficient);
	this->rightMovement *= (1.0f - deltaTime * slideCoefficient);
	this->leftMovement *= (1.0f - deltaTime * slideCoefficient);
	this->upMovement *= (1.0f - deltaTime * slideCoefficient);
	this->downMovement *= (1.0f - deltaTime * slideCoefficient);
}
