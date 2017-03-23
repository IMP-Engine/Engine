#pragma once
#include "input.h"




namespace input {

	std::vector< Observer* > observers;

	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		for (Observer* o : observers)
			o->keyCallback(window, key, scancode, action, mods);
	}

	void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
		if (!ImGui::GetIO().WantCaptureMouse)
			for (Observer* o : observers)
				o->mouseCallback(window, xpos, ypos);
	}

	void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
		ImGui_ImplGlfwGL3_MouseButtonCallback(window, button, action, mods);
		for (Observer* o : observers)
			o->mouseButtonCallback(window, button, action, mods);
	}

	void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
		ImGui_ImplGlfwGL3_ScrollCallback(window, xoffset, yoffset);
		for (Observer* o : observers)
			o->mouseCallback(window, xoffset, yoffset);
	}

	void initialize(GLFWwindow* window) {
		glfwSetKeyCallback(window, keyCallback);
		glfwSetCursorPosCallback(window, mouseCallback);
		glfwSetScrollCallback(window, scrollCallback);
		glfwSetMouseButtonCallback(window, mouseButtonCallback);
	}

	void subscribeObserver(Observer* o) {
		observers.push_back(o);
	}

	bool(&getKeys())[512]
	{
		return ImGui::GetIO().KeysDown;
	}
	
}