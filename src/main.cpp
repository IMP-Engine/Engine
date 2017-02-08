#include "glad\glad.h"
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include "imgui\imgui.h"
#include "imgui\imgui_impl_glfw_gl3.h"
#include <iostream>
#include "debug.h"


#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))


static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(void)
{
    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#if _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window)
    {
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

	ImVec4 clear_color = ImColor(164, 164, 164);
	ImGui_ImplGlfwGL3_Init(window, true); 

	// steal callback and call imgui in our callback
	glfwSetKeyCallback(window, key_callback);

    while (!glfwWindowShouldClose(window))
    {
		glfwPollEvents();
		ImGui_ImplGlfwGL3_NewFrame();

		// Consider scapping incase of performance
		static bool vsync = true;
		glfwSwapInterval(vsync ? 1 : 0);

		{ // GUI
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
			ImGui::End();

			// Demo window
			if (show_demo_window)
			{
				ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
				ImGui::ShowTestWindow(&show_demo_window);
			}
		}

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		
		ImGui::Render();
        glfwSwapBuffers(window);
    }

	// Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();
	ImGui_ImplGlfwGL3_Shutdown();
    exit(EXIT_SUCCESS);
}

