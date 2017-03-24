#pragma once
#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"
#include <vector>


namespace input {

	class Observer {
	public:
		virtual void keyCallback(GLFWwindow*, int, int, int, int) = 0;
		virtual void mouseCallback(GLFWwindow*, double, double) = 0;
		virtual void mouseButtonCallback(GLFWwindow*, int, int, int) = 0;
		virtual void mouseScrollCallback(GLFWwindow*, double, double) = 0;
	};

	void initialize(GLFWwindow*);

	void subscribeObserver(Observer*);

	bool (&getKeys())[512];

}