#include "lvn_glfw.h"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace lvn
{
	static bool s_glfwInit = false;

	static void GLFWerrorCallback(int error, const char* descripion)
	{
		LVN_CORE_ERROR("glfw-error: (%d): %s", error, descripion);
	}

	LvnResult glfwImplInitWindowContext(LvnWindowContext* windowContext)
	{
		if (s_glfwInit)
		{
			LVN_CORE_WARN("glfw already initialized!");
			return Lvn_Result_AlreadyCalled;
		}

		int success = glfwInit();
		if (!success)
		{
			LVN_CORE_ASSERT(success, "Failed to initialize glfw");
			return Lvn_Result_Failure;
		}
		glfwSetErrorCallback(GLFWerrorCallback);
		s_glfwInit = true;

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


		LvnGraphicsApi graphicsapi = getGraphicsApi();
		if (graphicsapi == Lvn_GraphicsApi_vulkan)
		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		}

		return Lvn_Result_Success;
	}

	void glfwImplTerminateWindowContext()
	{
		if (s_glfwInit)
		{
			glfwTerminate();
			s_glfwInit = false;
		}
		else LVN_CORE_WARN("glfw already terminated!");
	}

	void glfwImplCreateWindowInfo(LvnWindow* window, LvnWindowCreateInfo* winCreateInfo)
	{
		window->data.width = winCreateInfo->width;
		window->data.height = winCreateInfo->height;
		window->data.title = winCreateInfo->title;
		window->data.minWidth = winCreateInfo->minWidth;
		window->data.minHeight = winCreateInfo->minHeight;
		window->data.maxWidth = winCreateInfo->maxWidth;
		window->data.maxHeight = winCreateInfo->maxHeight;
		window->data.fullscreen = winCreateInfo->fullscreen;
		window->data.resizable = winCreateInfo->resizable;
		window->data.vSync = winCreateInfo->vSync;
		window->data.pIcons = winCreateInfo->pIcons;
		window->data.iconCount = winCreateInfo->iconCount;
		window->data.eventCallBackFn = glfwImplEventCallBackFn;

		glfwImplInitWindow(window);
	}

	void glfwImplInitWindow(LvnWindow* window)
	{
		GLFWmonitor* fullScreen = nullptr;
		if (window->data.fullscreen)
			fullScreen = glfwGetPrimaryMonitor();

		if (window->data.resizable)
			glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		else if (!window->data.resizable)
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		LVN_CORE_INFO("creating window: \"%s\" (w:%d, h:%d)", window->data.title, window->data.width, window->data.height);
		GLFWwindow* nativeWindow = glfwCreateWindow(window->data.width, window->data.height, window->data.title, fullScreen, nullptr);
		LVN_CORE_ASSERT(nativeWindow, "window failed to load!");

		if (window->data.pIcons != nullptr)
		{
			LvnVector<GLFWimage> images(window->data.iconCount);
			for (uint32_t i = 0; i < window->data.iconCount; i++)
			{
				GLFWimage image{};
				image.pixels = window->data.pIcons[i].image;
				image.width = window->data.pIcons[i].width;
				image.height = window->data.pIcons[i].height;
				images.push_back(image);
			}

			glfwSetWindowIcon(nativeWindow, static_cast<int>(window->data.iconCount), images.data());
		}

		glfwSetWindowSizeLimits(nativeWindow, window->data.minWidth, window->data.minHeight, window->data.maxWidth, window->data.maxHeight);
		glfwSetWindowUserPointer(nativeWindow, &window->data);

		window->nativeWindow = nativeWindow;

		// Set GLFW Callbacks
		glfwSetWindowSizeCallback(nativeWindow, [](GLFWwindow* window, int width, int height)
			{
				LvnWindowData& data = *(LvnWindowData*)glfwGetWindowUserPointer(window);
				data.width = width;
				data.height = height;

				LvnEvent event{};
				event.type = Lvn_EventType_WindowResize;
				event.category = Lvn_EventCategory_Window;
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
				LvnWindowData& data = *(LvnWindowData*)glfwGetWindowUserPointer(window);
				data.width = width;
				data.height = height;

				LvnEvent event{};
				event.type = Lvn_EventType_WindowFramebufferResize;
				event.category = Lvn_EventCategory_Window;
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
				LvnWindowData& data = *(LvnWindowData*)glfwGetWindowUserPointer(window);
				LvnEvent event{};
				event.type = Lvn_EventType_WindowMoved;
				event.category = Lvn_EventCategory_Window;
				event.handled = false;
				event.data.x = x;
				event.data.y = y;

				data.eventCallBackFn(&event);
			});

		glfwSetWindowFocusCallback(nativeWindow, [](GLFWwindow* window, int focused)
			{
				LvnWindowData& data = *(LvnWindowData*)glfwGetWindowUserPointer(window);
				if (focused)
				{
					LvnEvent event{};
					event.type = Lvn_EventType_WindowFocus;
					event.category = Lvn_EventCategory_Window;
					event.handled = false;

					data.eventCallBackFn(&event);
				}
				else
				{
					LvnEvent event{};
					event.type = Lvn_EventType_WindowLostFocus;
					event.category = Lvn_EventCategory_Window;
					event.handled = false;

					data.eventCallBackFn(&event);
				}
			});

		glfwSetWindowCloseCallback(nativeWindow, [](GLFWwindow* window)
			{
				LvnWindowData& data = *(LvnWindowData*)glfwGetWindowUserPointer(window);
				LvnEvent event{};
				event.type = Lvn_EventType_WindowClose;
				event.category = Lvn_EventCategory_Window;
				event.handled = false;

				data.eventCallBackFn(&event);
			});

		glfwSetKeyCallback(nativeWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				LvnWindowData& data = *(LvnWindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
					case GLFW_PRESS:
					{
						LvnEvent event{};
						event.type = Lvn_EventType_KeyPressed;
						event.category = Lvn_EventCategory_Input | Lvn_EventCategory_Keyboard;
						event.handled = false;
						event.data.code = key;
						event.data.repeat = false;
						data.eventCallBackFn(&event);
						break;
					}
					case GLFW_RELEASE:
					{
						LvnEvent event{};
						event.type = Lvn_EventType_KeyReleased;
						event.category = Lvn_EventCategory_Input | Lvn_EventCategory_Keyboard;
						event.handled = false;
						event.data.code = key;
						event.data.repeat = false;
						data.eventCallBackFn(&event);
						break;
					}
					case GLFW_REPEAT:
					{
						LvnEvent event{};
						event.type = Lvn_EventType_KeyHold;
						event.category = Lvn_EventCategory_Input | Lvn_EventCategory_Keyboard;
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
				LvnWindowData& data = *(LvnWindowData*)glfwGetWindowUserPointer(window);
				LvnEvent event{};
				event.type = Lvn_EventType_KeyTyped;
				event.category = Lvn_EventCategory_Input | Lvn_EventCategory_Keyboard;
				event.handled = false;
				event.data.ucode = keycode;
				data.eventCallBackFn(&event);
			});

		glfwSetMouseButtonCallback(nativeWindow, [](GLFWwindow* window, int button, int action, int mods)
			{
				LvnWindowData& data = *(LvnWindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
					case GLFW_PRESS:
					{
						LvnEvent event{};
						event.type = Lvn_EventType_MouseButtonPressed;
						event.category = Lvn_EventCategory_Input | Lvn_EventCategory_Mouse | Lvn_EventCategory_MouseButton;
						event.handled = false;
						event.data.code = button;
						data.eventCallBackFn(&event);
						break;
					}
					case GLFW_RELEASE:
					{
						LvnEvent event{};
						event.type = Lvn_EventType_MouseButtonReleased;
						event.category = Lvn_EventCategory_Input | Lvn_EventCategory_Mouse | Lvn_EventCategory_MouseButton;
						event.handled = false;
						event.data.code = button;
						data.eventCallBackFn(&event);
						break;
					}
				}
			});

		glfwSetScrollCallback(nativeWindow, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				LvnWindowData& data = *(LvnWindowData*)glfwGetWindowUserPointer(window);

				LvnEvent event{};
				event.type = Lvn_EventType_MouseScrolled;
				event.category = Lvn_EventCategory_Input | Lvn_EventCategory_Mouse | Lvn_EventCategory_MouseButton;
				event.handled = false;
				event.data.xd = xOffset;
				event.data.yd = yOffset;
				data.eventCallBackFn(&event);
			});

		glfwSetCursorPosCallback(nativeWindow, [](GLFWwindow* window, double xPos, double yPos)
			{
				LvnWindowData& data = *(LvnWindowData*)glfwGetWindowUserPointer(window);
				LvnEvent event{};
				event.type = Lvn_EventType_MouseMoved;
				event.category = Lvn_EventCategory_Input | Lvn_EventCategory_Mouse;
				event.handled = false;
				event.data.xd = xPos;
				event.data.yd = yPos;
				data.eventCallBackFn(&event);
			});
	}

	void glfwImplUpdateWindow(LvnWindow* window)
	{
		glfwSwapBuffers(static_cast<GLFWwindow*>(window->nativeWindow));
		glfwPollEvents();
	}

	bool glfwImplWindowOpen(LvnWindow* window)
	{
		return (!glfwWindowShouldClose(static_cast<GLFWwindow*>(window->nativeWindow)));
	}

	LvnWindowDimensions glfwImplGetDimensions(LvnWindow* window)
	{
		int width, height;
		glfwGetWindowSize(static_cast<GLFWwindow*>(window->nativeWindow), &width, &height);
		return { width, height };
	}

	unsigned int glfwImplGetWindowWidth(LvnWindow* window)
	{
		int width, height;
		glfwGetWindowSize(static_cast<GLFWwindow*>(window->nativeWindow), &width, &height);
		return width;
	}

	unsigned int glfwImplGetWindowHeight(LvnWindow* window)
	{
		int width, height;
		glfwGetWindowSize(static_cast<GLFWwindow*>(window->nativeWindow), &width, &height);
		return height;
	}

	void glfwImplSetWindowVSync(LvnWindow* window, bool enable)
	{
		// opengl
		glfwSwapInterval(enable);
		window->data.vSync = enable;
	}

	bool glfwImplGetWindowVSync(LvnWindow* window)
	{
		return window->data.vSync;
	}

	void glfwImplSetWindowContextCurrent(LvnWindow* window)
	{
		glfwMakeContextCurrent(static_cast<GLFWwindow*>(window->nativeWindow));
	}

	void glfwImplDestroyWindow(LvnWindow* window)
	{
		glfwDestroyWindow(static_cast<GLFWwindow*>(window->nativeWindow));
	}

	void glfwImplEventCallBackFn(LvnEvent* e)
	{
		return;
	}
}
