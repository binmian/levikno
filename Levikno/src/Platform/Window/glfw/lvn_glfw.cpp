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
			LVN_CORE_ERROR("glfw-error: (%d): %s", error, descripion);
		}

		void glfwInitWindowContext(WindowContext* windowContext)
		{
			if (!s_glfwInit)
			{
				int success = glfwInit();
				LVN_CORE_ASSERT(success, "Failed to initialize glfw");
				glfwSetErrorCallback(GLFWerrorCallback);
				s_glfwInit = true;

				windowContext->createWindow = glfwImplCreateWindow;
				windowContext->createWindowInfo = glfwImplCreateWindowInfo;
				windowContext->updateWindow	= glfwImplUpdateWindow;
				windowContext->windowOpen = glfwImplWindowOpen;
				windowContext->getDimensions = glfwImplGetDimensions;
				windowContext->getWindowWidth = glfwImplGetWindowWidth;
				windowContext->getWindowHeight = glfwImplGetWindowHeight;
				windowContext->setWindowVSync = glfwImplSetWindowVSync;
				windowContext->getWindowVSync = glfwImplGetWindowVSync;
				windowContext->setWindowContextCurrent = glfwImplSetWindowContextCurrent;
				windowContext->destroyWindow = glfwImplDestroyWindow;
			}			   
			else LVN_CORE_WARN("glfw already initialized!");
		}

		void glfwTerminateWindowContext()
		{
			if (s_glfwInit)
			{
				glfwTerminate();
				s_glfwInit = false;
			}
			else LVN_CORE_WARN("glfw already terminated!");
		}

		Window* glfwImplCreateWindow(int width, int height, const char* title, bool fullscreen, bool resizable, int minWidth, int minHeight)
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
			window->info.eventCallBackFn = glfwImplEventCallBackFn;

			glfwInitWindow(window);

			return window;
		}

		Window* glfwImplCreateWindowInfo(WindowCreateInfo* winCreateInfo)
		{
			Window* window = new Window();
			window->info.width = winCreateInfo->width;
			window->info.height = winCreateInfo->height;
			window->info.title = winCreateInfo->title;
			window->info.minWidth = winCreateInfo->minWidth;
			window->info.minHeight = winCreateInfo->minHeight;
			window->info.maxWidth = winCreateInfo->maxWidth;
			window->info.maxHeight = winCreateInfo->maxHeight;
			window->info.fullscreen = winCreateInfo->fullscreen;
			window->info.resizable = winCreateInfo->resizable;
			window->info.vSync = winCreateInfo->vSync;
			window->info.pIcons = winCreateInfo->pIcons;
			window->info.iconCount = winCreateInfo->iconCount;
			window->info.eventCallBackFn = glfwImplEventCallBackFn;

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

			LVN_CORE_INFO("creating window: %s - (w:%d, h:%d)", window->info.title, window->info.width, window->info.height);
			GLFWwindow* nativeWindow = glfwCreateWindow(window->info.width, window->info.height, window->info.title, fullScreen, nullptr);
			LVN_CORE_ASSERT(nativeWindow, "window failed to load!");

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
			glfwSwapInterval(window->info.vSync);
			glfwSetWindowUserPointer(nativeWindow, &window->info);

			window->nativeWindow = nativeWindow;

			// Set GLFW Callbacks
			glfwSetWindowSizeCallback(nativeWindow, [](GLFWwindow* window, int width, int height)
				{
					WindowInfo& data = *(WindowInfo*)glfwGetWindowUserPointer(window);
					data.width = width;
					data.height = height;

					Event event{};
					event.type = EventType::WindowResize;
					event.category = LvnEventCategory_Window;
					event.handled = false;
					event.data.x = width;
					event.data.y = height;

					data.eventCallBackFn(&event);

					/*switch (getGraphicsContext())
					{
						case GraphicsContext::OpenGL:
						{
							gladUpdateViewPort(window, width, height);
							break;
						}
						case GraphicsContext::Vulkan:
						{
							getVulkanBackends()->framebufferResized = true;
							break;
						}
					}*/
				});

			glfwSetFramebufferSizeCallback(nativeWindow, [](GLFWwindow* window, int width, int height)
				{
					WindowInfo& data = *(WindowInfo*)glfwGetWindowUserPointer(window);
					data.width = width;
					data.height = height;

					Event event{};
					event.type = EventType::WindowFramebufferResize;
					event.category = LvnEventCategory_Window;
					event.handled = false;
					event.data.x = width;
					event.data.y = height;

					data.eventCallBackFn(&event);

					/*switch (getGraphicsContext())
					{
					case GraphicsContext::OpenGL:
					{
						gladUpdateViewPort(window, width, height);
						break;
					}
					case GraphicsContext::Vulkan:
					{
						getVulkanBackends()->framebufferResized = true;
						break;
					}
					}*/
				});

			glfwSetWindowPosCallback(nativeWindow, [](GLFWwindow* window, int x, int y)
				{
					WindowInfo& data = *(WindowInfo*)glfwGetWindowUserPointer(window);
					Event event{};
					event.type = EventType::WindowMoved;
					event.category = LvnEventCategory_Window;
					event.handled = false;
					event.data.x = x;
					event.data.y = y;

					data.eventCallBackFn(&event);
				});

			glfwSetWindowFocusCallback(nativeWindow, [](GLFWwindow* window, int focused)
				{
					WindowInfo& data = *(WindowInfo*)glfwGetWindowUserPointer(window);
					if (focused)
					{
						Event event{};
						event.type = EventType::WindowFocus;
						event.category = LvnEventCategory_Window;
						event.handled = false;

						data.eventCallBackFn(&event);
					}
					else
					{
						Event event{};
						event.type = EventType::WindowLostFocus;
						event.category = LvnEventCategory_Window;
						event.handled = false;

						data.eventCallBackFn(&event);
					}
				});

			glfwSetWindowCloseCallback(nativeWindow, [](GLFWwindow* window)
				{
					WindowInfo& data = *(WindowInfo*)glfwGetWindowUserPointer(window);
					Event event{};
					event.type = EventType::WindowClose;
					event.category = LvnEventCategory_Window;
					event.handled = false;

					data.eventCallBackFn(&event);
				});

			glfwSetKeyCallback(nativeWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)
				{
					WindowInfo& data = *(WindowInfo*)glfwGetWindowUserPointer(window);

					switch (action)
					{
						case GLFW_PRESS:
						{
							Event event{};
							event.type = EventType::KeyPressed;
							event.category = LvnEventCategory_Input | LvnEventCategory_Keyboard;
							event.handled = false;
							event.data.code = key;
							event.data.repeat = false;
							data.eventCallBackFn(&event);
							break;
						}
						case GLFW_RELEASE:
						{
							Event event{};
							event.type = EventType::KeyReleased;
							event.category = LvnEventCategory_Input | LvnEventCategory_Keyboard;
							event.handled = false;
							event.data.code = key;
							event.data.repeat = false;
							data.eventCallBackFn(&event);
							break;
						}
						case GLFW_REPEAT:
						{
							Event event{};
							event.type = EventType::KeyHold;
							event.category = LvnEventCategory_Input | LvnEventCategory_Keyboard;
							event.handled = false;
							event.data.code = key;
							event.data.repeat = true;
							data.eventCallBackFn(&event);
							break;
						}
					}
				});

			glfwSetCharCallback(nativeWindow, [](GLFWwindow* window, unsigned int keycode)
				{
					WindowInfo& data = *(WindowInfo*)glfwGetWindowUserPointer(window);
					Event event{};
					event.type = EventType::KeyTyped;
					event.category = LvnEventCategory_Input | LvnEventCategory_Keyboard;
					event.handled = false;
					event.data.ucode = keycode;
					data.eventCallBackFn(&event);
				});

			glfwSetMouseButtonCallback(nativeWindow, [](GLFWwindow* window, int button, int action, int mods)
				{
					WindowInfo& data = *(WindowInfo*)glfwGetWindowUserPointer(window);

					switch (action)
					{
						case GLFW_PRESS:
						{
							Event event{};
							event.type = EventType::MouseButtonPressed;
							event.category = LvnEventCategory_Input | LvnEventCategory_Mouse | LvnEventCategory_MouseButton;
							event.handled = false;
							event.data.code = button;
							data.eventCallBackFn(&event);
							break;
						}
						case GLFW_RELEASE:
						{
							Event event{};
							event.type = EventType::MouseButtonReleased;
							event.category = LvnEventCategory_Input | LvnEventCategory_Mouse | LvnEventCategory_MouseButton;
							event.handled = false;
							event.data.code = button;
							data.eventCallBackFn(&event);
							break;
						}
					}
				});

			glfwSetScrollCallback(nativeWindow, [](GLFWwindow* window, double xOffset, double yOffset)
				{
					WindowInfo& data = *(WindowInfo*)glfwGetWindowUserPointer(window);

					Event event{};
					event.type = EventType::MouseScrolled;
					event.category = LvnEventCategory_Input | LvnEventCategory_Mouse | LvnEventCategory_MouseButton;
					event.handled = false;
					event.data.xd = xOffset;
					event.data.yd = yOffset;
					data.eventCallBackFn(&event);
				});

			glfwSetCursorPosCallback(nativeWindow, [](GLFWwindow* window, double xPos, double yPos)
				{
					WindowInfo& data = *(WindowInfo*)glfwGetWindowUserPointer(window);
					Event event{};
					event.type = EventType::MouseMoved;
					event.category = LvnEventCategory_Input | LvnEventCategory_Mouse;
					event.handled = false;
					event.data.xd = xPos;
					event.data.yd = yPos;
					data.eventCallBackFn(&event);
				});
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

		WindowDimension glfwImplGetDimensions(Window* window)
		{
			int width, height;
			glfwGetWindowSize(static_cast<GLFWwindow*>(window->nativeWindow), &width, &height);
			return { width, height };
		}

		unsigned int glfwImplGetWindowWidth(Window* window)
		{
			int width, height;
			glfwGetWindowSize(static_cast<GLFWwindow*>(window->nativeWindow), &width, &height);
			return width;
		}

		unsigned int glfwImplGetWindowHeight(Window* window)
		{
			int width, height;
			glfwGetWindowSize(static_cast<GLFWwindow*>(window->nativeWindow), &width, &height);
			return height;
		}

		void glfwImplSetWindowVSync(Window* window, bool enable)
		{
			// opengl
			glfwSwapInterval(enable);
			window->info.vSync = enable;
		}

		bool glfwImplGetWindowVSync(Window* window)
		{
			return window->info.vSync;
		}

		void glfwImplSetWindowContextCurrent(Window* window)
		{
			glfwMakeContextCurrent(static_cast<GLFWwindow*>(window->nativeWindow));
		}

		void glfwImplDestroyWindow(Window* window)
		{
			glfwDestroyWindow(static_cast<GLFWwindow*>(window->nativeWindow));
		}

		void glfwImplEventCallBackFn(Event* e)
		{
			return;
		}

	}
}