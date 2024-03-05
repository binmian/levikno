#include "lvn_glfw.h"

#include "levikno.h"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "lvn_vulkanBackends.h"

namespace lvn
{
	static bool s_glfwInit = false;

	static void      GLFWerrorCallback(int error, const char* descripion);
	static LvnResult createGraphicsRelatedAPIData(LvnWindow* window);
	static void      destroyGraphicsRelatedAPIData(LvnWindow* window);

	static void GLFWerrorCallback(int error, const char* descripion)
	{
		LVN_CORE_ERROR("glfw-error: (%d): %s", error, descripion);
	}

	static LvnResult createGraphicsRelatedAPIData(LvnWindow* window)
	{
		switch (lvn::getGraphicsApi())
		{
			case Lvn_GraphicsApi_vulkan:
			{
				createVulkanWindowSurfaceData(window);
				return Lvn_Result_Success;
			}

			default:
			{
				return Lvn_Result_Success;
			}
		}
	}

	static void destroyGraphicsRelatedAPIData(LvnWindow* window)
	{
		switch (lvn::getGraphicsApi())
		{
			case Lvn_GraphicsApi_vulkan:
			{
				destroyVulkanWindowSurfaceData(window);
			}

			default:
			{
				return;
			}
		}
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

		windowContext->createWindow = glfwImplCreateWindow;
		windowContext->destroyWindow = glfwImplDestroyWindow;
		windowContext->updateWindow	= glfwImplUpdateWindow;
		windowContext->windowOpen = glfwImplWindowOpen;
		windowContext->getDimensions = glfwImplGetDimensions;
		windowContext->getWindowWidth = glfwImplGetWindowWidth;
		windowContext->getWindowHeight = glfwImplGetWindowHeight;
		windowContext->setWindowVSync = glfwImplSetWindowVSync;
		windowContext->getWindowVSync = glfwImplGetWindowVSync;
		windowContext->setWindowContextCurrent = glfwImplSetWindowContextCurrent;


		LvnGraphicsApi graphicsapi = lvn::getGraphicsApi();
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

	LvnResult glfwImplCreateWindow(LvnWindow* window, LvnWindowCreateInfo* createInfo)
	{
		window->data.width = createInfo->width;
		window->data.height = createInfo->height;
		window->data.title = createInfo->title;
		window->data.minWidth = createInfo->minWidth;
		window->data.minHeight = createInfo->minHeight;
		window->data.maxWidth = createInfo->maxWidth;
		window->data.maxHeight = createInfo->maxHeight;
		window->data.fullscreen = createInfo->fullscreen;
		window->data.resizable = createInfo->resizable;
		window->data.vSync = createInfo->vSync;
		window->data.pIcons = createInfo->pIcons;
		window->data.iconCount = createInfo->iconCount;
		window->data.eventCallBackFn = glfwImplEventCallBackFn;

		GLFWmonitor* fullScreen = nullptr;
		if (window->data.fullscreen)
			fullScreen = glfwGetPrimaryMonitor();

		if (window->data.resizable)
			glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		else
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		GLFWwindow* nativeWindow = glfwCreateWindow(window->data.width, window->data.height, window->data.title, fullScreen, nullptr);
		LVN_CORE_TRACE("[glfw] created window <GLFWwindow*> (%p): \"%s\" (w:%d, h:%d)", nativeWindow, window->data.title, window->data.width, window->data.height);

		if (!nativeWindow)
		{
			LVN_CORE_ERROR("failed to create window: \"%s\" (w:%d, h:%d)", window->data.title, window->data.width, window->data.height);
			return Lvn_Result_Failure;
		}

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
		glfwSetWindowUserPointer(nativeWindow, window);

		window->nativeWindow = nativeWindow;
		if (createGraphicsRelatedAPIData(window) != Lvn_Result_Success)
		{
			LVN_CORE_ERROR("[glfw] failed to create graphics api related data for window: (%p), native glfw window <GLFWwindow*> (%p)", window, nativeWindow);
			return Lvn_Result_Failure;
		}

		// Set GLFW Callbacks
		glfwSetWindowSizeCallback(nativeWindow, [](GLFWwindow* window, int width, int height)
		{
			LvnWindowData* data = &((LvnWindow*)glfwGetWindowUserPointer(window))->data;
			data->width = width;
			data->height = height;

			LvnEvent event{};
			event.type = Lvn_EventType_WindowResize;
			event.category = Lvn_EventCategory_Window;
			event.handled = false;
			event.data.x = width;
			event.data.y = height;

			data->eventCallBackFn(&event);
		});

		glfwSetFramebufferSizeCallback(nativeWindow, [](GLFWwindow* window, int width, int height)
		{
			LvnWindow* lvnWindow = ((LvnWindow*)glfwGetWindowUserPointer(window));
			LvnWindowData* data = &lvnWindow->data;
			data->width = width;
			data->height = height;

			LvnEvent event{};
			event.type = Lvn_EventType_WindowFramebufferResize;
			event.category = Lvn_EventCategory_Window;
			event.handled = false;
			event.data.x = width;
			event.data.y = height;

			data->eventCallBackFn(&event);

			switch (lvn::getGraphicsApi())
			{
				case Lvn_GraphicsApi_opengl:
				{
					// gladUpdateViewPort(window, width, height);
					break;
				}
				case Lvn_GraphicsApi_vulkan:
				{
					VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(lvnWindow->apiData);
					surfaceData->frameBufferResized = true;
					break;
				}
				default:
				{
					break;
				}
			}
		});

		glfwSetWindowPosCallback(nativeWindow, [](GLFWwindow* window, int x, int y)
		{
			LvnWindowData* data = &((LvnWindow*)glfwGetWindowUserPointer(window))->data;
			LvnEvent event{};
			event.type = Lvn_EventType_WindowMoved;
			event.category = Lvn_EventCategory_Window;
			event.handled = false;
			event.data.x = x;
			event.data.y = y;

			data->eventCallBackFn(&event);
		});

		glfwSetWindowFocusCallback(nativeWindow, [](GLFWwindow* window, int focused)
		{
			LvnWindowData* data = &((LvnWindow*)glfwGetWindowUserPointer(window))->data;
			if (focused)
			{
				LvnEvent event{};
				event.type = Lvn_EventType_WindowFocus;
				event.category = Lvn_EventCategory_Window;
				event.handled = false;

				data->eventCallBackFn(&event);
			}
			else
			{
				LvnEvent event{};
				event.type = Lvn_EventType_WindowLostFocus;
				event.category = Lvn_EventCategory_Window;
				event.handled = false;

				data->eventCallBackFn(&event);
			}
		});

		glfwSetWindowCloseCallback(nativeWindow, [](GLFWwindow* window)
		{
			LvnWindowData* data = &((LvnWindow*)glfwGetWindowUserPointer(window))->data;
			LvnEvent event{};
			event.type = Lvn_EventType_WindowClose;
			event.category = Lvn_EventCategory_Window;
			event.handled = false;

			data->eventCallBackFn(&event);
		});

		glfwSetKeyCallback(nativeWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			LvnWindowData* data = &((LvnWindow*)glfwGetWindowUserPointer(window))->data;

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
					data->eventCallBackFn(&event);
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
					data->eventCallBackFn(&event);
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
					data->eventCallBackFn(&event);
					break;
				}
			}
		});

		glfwSetCharCallback(nativeWindow, [](GLFWwindow* window, unsigned int keycode)
		{
			LvnWindowData* data = &((LvnWindow*)glfwGetWindowUserPointer(window))->data;
			LvnEvent event{};
			event.type = Lvn_EventType_KeyTyped;
			event.category = Lvn_EventCategory_Input | Lvn_EventCategory_Keyboard;
			event.handled = false;
			event.data.ucode = keycode;
			data->eventCallBackFn(&event);
		});

		glfwSetMouseButtonCallback(nativeWindow, [](GLFWwindow* window, int button, int action, int mods)
		{
			LvnWindowData* data = &((LvnWindow*)glfwGetWindowUserPointer(window))->data;

			switch (action)
			{
				case GLFW_PRESS:
				{
					LvnEvent event{};
					event.type = Lvn_EventType_MouseButtonPressed;
					event.category = Lvn_EventCategory_Input | Lvn_EventCategory_Mouse | Lvn_EventCategory_MouseButton;
					event.handled = false;
					event.data.code = button;
					data->eventCallBackFn(&event);
					break;
				}
				case GLFW_RELEASE:
				{
					LvnEvent event{};
					event.type = Lvn_EventType_MouseButtonReleased;
					event.category = Lvn_EventCategory_Input | Lvn_EventCategory_Mouse | Lvn_EventCategory_MouseButton;
					event.handled = false;
					event.data.code = button;
					data->eventCallBackFn(&event);
					break;
				}
			}
		});

		glfwSetScrollCallback(nativeWindow, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			LvnWindowData* data = &((LvnWindow*)glfwGetWindowUserPointer(window))->data;

			LvnEvent event{};
			event.type = Lvn_EventType_MouseScrolled;
			event.category = Lvn_EventCategory_Input | Lvn_EventCategory_Mouse | Lvn_EventCategory_MouseButton;
			event.handled = false;
			event.data.xd = xOffset;
			event.data.yd = yOffset;
			data->eventCallBackFn(&event);
		});

		glfwSetCursorPosCallback(nativeWindow, [](GLFWwindow* window, double xPos, double yPos)
		{
			LvnWindowData* data = &((LvnWindow*)glfwGetWindowUserPointer(window))->data;
			LvnEvent event{};
			event.type = Lvn_EventType_MouseMoved;
			event.category = Lvn_EventCategory_Input | Lvn_EventCategory_Mouse;
			event.handled = false;
			event.data.xd = xPos;
			event.data.yd = yPos;
			data->eventCallBackFn(&event);
		});

		return Lvn_Result_Success;
	}

	void glfwImplUpdateWindow(LvnWindow* window)
	{
		if (lvn::getGraphicsApi() == Lvn_GraphicsApi_opengl)
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
		destroyGraphicsRelatedAPIData(window);
		glfwDestroyWindow(static_cast<GLFWwindow*>(window->nativeWindow));
	}

	void glfwImplEventCallBackFn(LvnEvent* e)
	{
		return;
	}
}
