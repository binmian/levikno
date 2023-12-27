#ifndef HG_LEVIKNO_GLFW_H
#define HG_LEVIKNO_GLFW_H

#include "Core/core_internal.h"

namespace lvn
{
	namespace glfws
	{
		void glfwInitWindowContext();
		void glfwTerminateWindowContext();

		Window* glfwImplCreateWindow(uint32_t width, uint32_t height, const char* title, bool fullscreen = false, bool resizable = true, int minWidth = 0, int minHeight = 0);
		Window* glfwImplCreateWindow(WindowInfo winCreateInfo);
		void glfwInitWindow(Window* window);

		void glfwImplUpdateWindow(Window* window);
		bool glfwImplWindowOpen(Window* window);
	}
}

#endif // !HG_CHONPS_GLFW_H
