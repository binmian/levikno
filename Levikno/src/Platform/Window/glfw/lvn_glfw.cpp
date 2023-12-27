#include "lvn_glfw.h"

#include <stdlib.h>
#include <vector>

#define GLFW_INCLUDE_NONE
#include <glfw/glfw3.h>
#include <glad/glad.h>

namespace lvn
{
	namespace glfws
	{
		static bool s_glfwInit = false;

		static void GLFWerrorCallback(int error, const char* descripion)
		{
			LVN_CORE_LOG_ERROR(glfw, "(%d): {%s}", error, descripion);
		}

		void glfwInitWindowContext()
		{
			if (!s_glfwInit)
			{
				int success = glfwInit();
				LVN_CORE_ASSERT(success, "Failed to initialize glfw");
				glfwSetErrorCallback(GLFWerrorCallback);
				s_glfwInit = true;
			}
			else LVN_CORE_LOG_WARN(glfw, "glfw already initialized!");
		}

		void glfwTerminateWindowContext()
		{
			if (s_glfwInit)
			{
				glfwTerminate();
				s_glfwInit = false;
			}
			else LVN_CORE_LOG_WARN(glfw, "glfw already terminated!");
		}

		Window* glfwImplCreateWindow(uint32_t width, uint32_t height, const char* title, bool fullscreen, bool resizable, int minWidth, int minHeight)
		{
			Window* window = new Window();
			window->info.width = width;
			window->info.height = height;
			window->info.title = title;
			window->info.fullscreen = fullscreen;
			window->info.resizable = resizable;
			window->info.minWidth = minWidth;
			window->info.minHeight = minHeight;
			window->info.maxWidth = -1;
			window->info.maxHeight = -1;
			window->updateWindow = glfwImplUpdateWindow;
			window->windowOpen = glfwImplWindowOpen;

			glfwInitWindow(window);

			return window;
		}

		Window* glfwImplCreateWindow(WindowInfo winCreateInfo)
		{
			Window* window = new Window();
			window->info = winCreateInfo;
			window->updateWindow = glfwImplUpdateWindow;
			window->windowOpen = glfwImplWindowOpen;

			glfwInitWindow(window);

			return window;
		}

		void glfwInitWindow(Window* window)
		{
			GLFWmonitor* fullScreen = nullptr;
			if (window->info.fullscreen)
				fullScreen = glfwGetPrimaryMonitor();

			/*GraphicsContext GraphicsContext = getGraphicsContext();
			if (GraphicsContext == GraphicsContext::Vulkan)
			{
				glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			}*/

			if (!window->info.resizable)
				glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

			LVN_CORE_INFO("Creating Window: %s - (w:%d, h:%d)", window->info.title, window->info.width, window->info.height);
			GLFWwindow* nativeWindow = glfwCreateWindow(window->info.width, window->info.height, window->info.title, fullScreen, nullptr);
			LVN_CORE_ASSERT(nativeWindow, "Window failed to load!");

			if (window->info.pIcons != nullptr)
			{
				std::vector<GLFWimage> images;
				for (int i = 0; i < window->info.iconCount; i++)
				{
					GLFWimage image{};
					image.pixels = window->info.pIcons[i].image;
					image.width = window->info.pIcons[i].width;
					image.height = window->info.pIcons[i].height;
					images.push_back(image);
				}

				glfwSetWindowIcon(nativeWindow, static_cast<int>(images.size()), images.data());
			}

			glfwSetWindowSizeLimits(nativeWindow, window->info.minWidth, window->info.minHeight, window->info.maxWidth, window->info.maxHeight);

			glfwSetWindowUserPointer(nativeWindow, &window->info);

			window->nativeWindow = nativeWindow;
		}

		void glfwImplUpdateWindow(Window* window)
		{
			glfwSwapBuffers(static_cast<GLFWwindow*>(window->nativeWindow));
			glfwPollEvents();
		}

		bool glfwImplWindowOpen(Window* window)
		{
			return (!glfwWindowShouldClose(static_cast<GLFWwindow*>(window->nativeWindow)));
		}
	}
}