#include "lvn_glfw.h"

#include <stdlib.h>

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>

#include "Graphics/graphics_internal.h"

namespace lvn
{
	static bool s_glfwInit = false;

	static void GLFWerrorCallback(int error, const char* descripion)
	{
		LVN_CORE_ERROR("glfw-error: (%d): %s", error, descripion);
	}

	void glfwImplInitWindowContext(WindowContext* windowContext)
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

	void glfwImplTerminateWindowContext()
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
		window->data.width = width;
		window->data.height = height;
		window->data.title = title;
		window->data.fullscreen = fullscreen;
		window->data.resizable = resizable;
		window->data.minWidth = minWidth;
		window->data.minHeight = minHeight;
		window->data.maxWidth = -1;
		window->data.maxHeight = -1;
		window->data.eventCallBackFn = glfwImplEventCallBackFn;

		glfwImplInitWindow(window);

		return window;
	}

	Window* glfwImplCreateWindowInfo(WindowCreateInfo* winCreateInfo)
	{
		Window* window = new Window();
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

		return window;
	}

	void glfwImplInitWindow(Window* window)
	{
		GLFWmonitor* fullScreen = nullptr;
		if (window->data.fullscreen)
			fullScreen = glfwGetPrimaryMonitor();

		GraphicsAPI graphicsapi = getGraphicsAPI();
		if (graphicsapi == GraphicsAPI::vulkan)
		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		}

		if (!window->data.resizable)
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		LVN_CORE_INFO("creating window: %s - (w:%d, h:%d)", window->data.title, window->data.width, window->data.height);
		GLFWwindow* nativeWindow = glfwCreateWindow(window->data.width, window->data.height, window->data.title, fullScreen, nullptr);
		LVN_CORE_ASSERT(nativeWindow, "window failed to load!");

		if (window->data.pIcons != nullptr)
		{
			GLFWimage* images = (GLFWimage*)malloc(window->data.iconCount * sizeof(GLFWimage));
			for (uint32_t i = 0; i < window->data.iconCount; i++)
			{
				GLFWimage image{};
				image.pixels = window->data.pIcons[i].image;
				image.width = window->data.pIcons[i].width;
				image.height = window->data.pIcons[i].height;
				images[i] = image;
			}

			glfwSetWindowIcon(nativeWindow, static_cast<int>(window->data.iconCount), images);
			free(images);
		}

		glfwSetWindowSizeLimits(nativeWindow, window->data.minWidth, window->data.minHeight, window->data.maxWidth, window->data.maxHeight);
		glfwSwapInterval(window->data.vSync);
		glfwSetWindowUserPointer(nativeWindow, &window->data);

		window->nativeWindow = nativeWindow;

		// Set GLFW Callbacks
		glfwSetWindowSizeCallback(nativeWindow, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
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
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
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
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
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
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
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
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				Event event{};
				event.type = EventType::WindowClose;
				event.category = LvnEventCategory_Window;
				event.handled = false;

				data.eventCallBackFn(&event);
			});

		glfwSetKeyCallback(nativeWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

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
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				Event event{};
				event.type = EventType::KeyTyped;
				event.category = LvnEventCategory_Input | LvnEventCategory_Keyboard;
				event.handled = false;
				event.data.ucode = keycode;
				data.eventCallBackFn(&event);
			});

		glfwSetMouseButtonCallback(nativeWindow, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

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
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

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
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
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
		window->data.vSync = enable;
	}

	bool glfwImplGetWindowVSync(Window* window)
	{
		return window->data.vSync;
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

	const char** glfwImplGetInstanceExtensions(uint32_t* extensionCount)
	{
		return glfwGetRequiredInstanceExtensions(extensionCount);
	}
}