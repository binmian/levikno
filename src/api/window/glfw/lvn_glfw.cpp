#include "lvn_glfw.h"

#include "levikno.h"
#include "lvn_opengl.h"

#if defined(LVN_GRAPHICS_API_INCLUDE_VULKAN)
	#include "lvn_vulkanBackends.h"
	#define GLFW_INCLUDE_NONE
	#define GLFW_INCLUDE_VULKAN
#endif
#include <GLFW/glfw3.h>

namespace lvn
{
	static bool s_glfwInit = false;
	static GLFWcursor* s_CursorIcons[10];

	static void      GLFWerrorCallback(int error, const char* descripion);
	static LvnResult createGraphicsRelatedAPIData(LvnWindow* window);
	static void      destroyGraphicsRelatedAPIData(LvnWindow* window);

	static void GLFWerrorCallback(int error, const char* descripion)
	{
		LVN_CORE_ERROR("[glfw]: (%d): %s", error, descripion);
	}

	static LvnResult createGraphicsRelatedAPIData(LvnWindow* window)
	{
		switch (lvn::getGraphicsApi())
		{
			case Lvn_GraphicsApi_vulkan:
			{
			#if defined(LVN_GRAPHICS_API_INCLUDE_VULKAN)
				lvn::createVulkanWindowSurfaceData(window);
			#else
				LVN_CORE_ASSERT(false, "vulkan graphics api not included on platform, cannot create vulkan related surface data");
			#endif
				return Lvn_Result_Success;
			}
			case Lvn_GraphicsApi_opengl:
			{
				glfwMakeContextCurrent(static_cast<GLFWwindow*>(window->nativeWindow));
				setOglWindowContextValues();
				return Lvn_Result_Success;
			}

			default:
			{
				return Lvn_Result_Failure;
			}
		}
	}

	static void destroyGraphicsRelatedAPIData(LvnWindow* window)
	{
		switch (lvn::getGraphicsApi())
		{
			case Lvn_GraphicsApi_vulkan:
			{
			#if defined(LVN_GRAPHICS_API_INCLUDE_VULKAN)
				lvn::destroyVulkanWindowSurfaceData(window);
			#endif
				break;
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
		s_glfwInit = true;

		windowContext->createWindow = glfwImplCreateWindow;
		windowContext->destroyWindow = glfwImplDestroyWindow;
		windowContext->updateWindow	= glfwImplUpdateWindow;
		windowContext->windowOpen = glfwImplWindowOpen;
		windowContext->windowPollEvents = glfwImplWindowPollEvents;
		windowContext->getDimensions = glfwImplGetDimensions;
		windowContext->getWindowWidth = glfwImplGetWindowWidth;
		windowContext->getWindowHeight = glfwImplGetWindowHeight;
		windowContext->setWindowVSync = glfwImplSetWindowVSync;
		windowContext->getWindowVSync = glfwImplGetWindowVSync;
		windowContext->setWindowContextCurrent = glfwImplSetWindowContextCurrent;
		windowContext->keyPressed = glfwImplKeyPressed;
		windowContext->keyReleased = glfwImplKeyReleased;
		windowContext->mouseButtonPressed = glfwImplMouseButtonPressed;
		windowContext->mouseButtonReleased = glfwImplMouseButtonReleased;

		windowContext->getMousePos = glfwImplGetMousePos;
		windowContext->getMousePosPtr = glfwImplGetMousePosPtr;
		windowContext->getMouseX = glfwImplGetMouseX;
		windowContext->getMouseY = glfwImplGetMouseY;
		windowContext->setMouseCursor = glfwImplSetMouseCursor;
		windowContext->SetMouseInputMode = glfwImplSetMouseInputMode;

		windowContext->getWindowPos = glfwImplGetWindowPos;
		windowContext->getWindowPosPtr = glfwImplGetWindowPosPtr;
		windowContext->getWindowSize = glfwImplGetWindowSize;
		windowContext->getWindowSizePtr = glfwImplGetWindowSizePtr;


		LvnGraphicsApi graphicsapi = lvn::getGraphicsApi();
		if (graphicsapi == Lvn_GraphicsApi_vulkan)
		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		}

		s_CursorIcons[Lvn_MouseCursor_Arrow] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
		s_CursorIcons[Lvn_MouseCursor_Ibeam] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
		s_CursorIcons[Lvn_MouseCursor_Crosshair] = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
		s_CursorIcons[Lvn_MouseCursor_PointingHand] = glfwCreateStandardCursor(GLFW_POINTING_HAND_CURSOR);
		s_CursorIcons[Lvn_MouseCursor_ResizeEW] = glfwCreateStandardCursor(GLFW_RESIZE_EW_CURSOR);
		s_CursorIcons[Lvn_MouseCursor_ResizeNS] = glfwCreateStandardCursor(GLFW_RESIZE_NS_CURSOR);
		s_CursorIcons[Lvn_MouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
		s_CursorIcons[Lvn_MouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
		s_CursorIcons[Lvn_MouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR);
		s_CursorIcons[Lvn_MouseCursor_NotAllowed] = glfwCreateStandardCursor(GLFW_NOT_ALLOWED_CURSOR);


		glfwSetErrorCallback(GLFWerrorCallback);

		return Lvn_Result_Success;
	}

	void glfwImplTerminateWindowContext()
	{
		for (uint32_t i = 0; i < sizeof(s_CursorIcons) / sizeof(s_CursorIcons[0]); i++)
		{
			glfwDestroyCursor(s_CursorIcons[i]);
		}

		if (s_glfwInit)
		{
			glfwTerminate();
			s_glfwInit = false;
		}
		else LVN_CORE_WARN("glfw already terminated!");
	}

	LvnResult glfwImplCreateWindow(LvnWindow* window, const LvnWindowCreateInfo* createInfo)
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

		if (createInfo->eventCallBack == nullptr)
			window->data.eventCallBackFn = glfwImplEventCallBackFn;
		else
			window->data.eventCallBackFn = createInfo->eventCallBack;

		window->data.userData = createInfo->userData;

		GLFWmonitor* fullScreen = nullptr;
		if (window->data.fullscreen)
			fullScreen = glfwGetPrimaryMonitor();

		if (window->data.resizable)
			glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		else
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		LvnGraphicsApi graphicsapi = lvn::getGraphicsApi();

		// get shared context (opengl)
		GLFWwindow* windowContext = nullptr;
		if (graphicsapi == Lvn_GraphicsApi_opengl)
			windowContext = static_cast<GLFWwindow*>(lvn::getMainOglWindowContext());

		// create window
		GLFWwindow* nativeWindow = glfwCreateWindow(window->data.width, window->data.height, window->data.title.c_str(), fullScreen, windowContext);
		LVN_CORE_TRACE("[glfw] created window <GLFWwindow*> (%p): \"%s\" (w:%d,h:%d)", nativeWindow, window->data.title.c_str(), window->data.width, window->data.height);

		if (!nativeWindow)
		{
			LVN_CORE_ERROR("failed to create window: \"%s\" (w:%d, h:%d)", window->data.title.c_str(), window->data.width, window->data.height);
			return Lvn_Result_Failure;
		}

		if (window->data.pIcons != nullptr)
		{
			std::vector<GLFWimage> images(window->data.iconCount);

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

		// set window graphics api related data (eg. swapchains, framebuffers)
		window->nativeWindow = nativeWindow;
		if (createGraphicsRelatedAPIData(window) != Lvn_Result_Success)
		{
			LVN_CORE_ERROR("[glfw] failed to create graphics api related data for window: (%p), native glfw window <GLFWwindow*> (%p)", window, nativeWindow);
			return Lvn_Result_Failure;
		}

		// set window size parameters & vsync
		glfwSetWindowSizeLimits(nativeWindow, window->data.minWidth, window->data.minHeight, window->data.maxWidth, window->data.maxHeight);
		glfwSetWindowUserPointer(nativeWindow, window);

		if (graphicsapi == Lvn_GraphicsApi_opengl)
			glfwSwapInterval(createInfo->vSync);

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
			event.userData = data->userData;

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
			event.userData = data->userData;

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
				#if defined(LVN_GRAPHICS_API_INCLUDE_VULKAN)
					VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(lvnWindow->apiData);
					surfaceData->frameBufferResized = true;
				#endif
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
			event.userData = data->userData;

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
				event.userData = data->userData;

				data->eventCallBackFn(&event);
			}
			else
			{
				LvnEvent event{};
				event.type = Lvn_EventType_WindowLostFocus;
				event.category = Lvn_EventCategory_Window;
				event.handled = false;
				event.userData = data->userData;

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
			event.userData = data->userData;

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
					event.userData = data->userData;
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
					event.userData = data->userData;
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
					event.userData = data->userData;
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
			event.userData = data->userData;
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
					event.userData = data->userData;
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
					event.userData = data->userData;
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
			event.userData = data->userData;
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
			event.userData = data->userData;
			data->eventCallBackFn(&event);
		});

		return Lvn_Result_Success;
	}

	void glfwImplUpdateWindow(LvnWindow* window)
	{
		if (lvn::getGraphicsApi() == Lvn_GraphicsApi_opengl)
			glfwSwapBuffers(static_cast<GLFWwindow*>(window->nativeWindow));
	}

	bool glfwImplWindowOpen(LvnWindow* window)
	{
		return (!glfwWindowShouldClose(static_cast<GLFWwindow*>(window->nativeWindow)));
	}

	void glfwImplWindowPollEvents()
	{
		glfwPollEvents();
	}

	LvnPair<int> glfwImplGetDimensions(LvnWindow* window)
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
		window->data.vSync = enable;

		switch (lvn::getGraphicsApi())
		{
			case Lvn_GraphicsApi_opengl:
			{
				glfwSwapInterval(enable);
				break;
			}
			case Lvn_GraphicsApi_vulkan:
			{
			#if defined(LVN_GRAPHICS_API_INCLUDE_VULKAN)
				VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(window->apiData);
				surfaceData->frameBufferResized = true; // set resize to true to recreate swapchain
			#endif
				break;
			}
		}
	}

	bool glfwImplGetWindowVSync(LvnWindow* window)
	{
		return window->data.vSync;
	}

	void glfwImplSetWindowContextCurrent(LvnWindow* window)
	{
		if (lvn::getGraphicsApi() == Lvn_GraphicsApi_opengl)
		{
			GLFWwindow* sharedContext = static_cast<GLFWwindow*>(lvn::getMainOglWindowContext());
			glfwMakeContextCurrent(window ? static_cast<GLFWwindow*>(window->nativeWindow) : sharedContext);
		}
	}

	void glfwImplDestroyWindow(LvnWindow* window)
	{
		destroyGraphicsRelatedAPIData(window);
		glfwDestroyWindow(static_cast<GLFWwindow*>(window->nativeWindow));
	}

	void glfwImplEventCallBackFn(LvnEvent* e) // default function for event call backs if no function is set
	{
		return;
	}

	bool glfwImplKeyPressed(LvnWindow* window, int keycode)
	{
		GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->nativeWindow);
		int state = glfwGetKey(glfwWin, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool glfwImplKeyReleased(LvnWindow* window, int keycode)
	{
		GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->nativeWindow);
		int state = glfwGetKey(glfwWin, keycode);
		return state == GLFW_RELEASE;
	}

	bool glfwImplMouseButtonPressed(LvnWindow* window, int button)
	{
		GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->nativeWindow);
		int state = glfwGetMouseButton(glfwWin, button);
		return state == GLFW_PRESS;
	}

	bool glfwImplMouseButtonReleased(LvnWindow* window, int button)
	{
		GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->nativeWindow);
		int state = glfwGetMouseButton(glfwWin, button);
		return state == GLFW_RELEASE;
	}

	LvnPair<float> glfwImplGetMousePos(LvnWindow* window)
	{
		GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->nativeWindow);
		double xpos, ypos;
		glfwGetCursorPos(glfwWin, &xpos, &ypos);
		return { (float)xpos, (float)ypos };
	}

	void glfwImplGetMousePosPtr(LvnWindow* window, float* xpos, float* ypos)
	{
		GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->nativeWindow);
		double xPos, yPos;
		glfwGetCursorPos(glfwWin, &xPos, &yPos);
		*xpos = (float)xPos;
		*ypos = (float)yPos;
	}

	float glfwImplGetMouseX(LvnWindow* window)
	{
		GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->nativeWindow);
		double xPos, yPos;
		glfwGetCursorPos(glfwWin, &xPos, &yPos);
		return (float)xPos;
	}

	float glfwImplGetMouseY(LvnWindow* window)
	{
		GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->nativeWindow);
		double xPos, yPos;
		glfwGetCursorPos(glfwWin, &xPos, &yPos);
		return (float)yPos;
	}

	void glfwImplSetMouseCursor(LvnWindow* window, LvnMouseCursor cursor)
	{
		LVN_CORE_ASSERT(static_cast<uint32_t>(cursor) < (sizeof(s_CursorIcons) / sizeof(s_CursorIcons[0])), "cursor mode index out of range");

		GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->nativeWindow);
		glfwSetCursor(glfwWin, s_CursorIcons[cursor]);
	}

	void glfwImplSetMouseInputMode(LvnWindow* window, LvnMouseInputMode mode)
	{
		GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->nativeWindow);
		auto modeEnum = GLFW_CURSOR_NORMAL;

		switch (mode)
		{
			case Lvn_MouseInputMode_Normal: { modeEnum = GLFW_CURSOR_NORMAL; break; }
			case Lvn_MouseInputMode_Disable: { modeEnum = GLFW_CURSOR_DISABLED; break; }
			case Lvn_MouseInputMode_Hidden: { modeEnum = GLFW_CURSOR_HIDDEN; break; }
			case Lvn_MouseInputMode_Captured: { modeEnum = GLFW_CURSOR_CAPTURED; break; }
		}

		glfwSetInputMode(glfwWin, GLFW_CURSOR, modeEnum);
	}

	LvnPair<int> glfwImplGetWindowPos(LvnWindow* window)
	{
		GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->nativeWindow);
		int xpos, ypos;
		glfwGetWindowPos(glfwWin, &xpos, &ypos);
		return { xpos, ypos };
	}

	void glfwImplGetWindowPosPtr(LvnWindow* window, int* xpos, int* ypos)
	{
		GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->nativeWindow);
		glfwGetWindowPos(glfwWin, &(*xpos), &(*ypos));
	}

	LvnPair<int> glfwImplGetWindowSize(LvnWindow* window)
	{
		GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->nativeWindow);
		int width, height;
		glfwGetWindowSize(glfwWin, &width, &height);
		return { width, height };
	}

	void glfwImplGetWindowSizePtr(LvnWindow* window, int* width, int* height)
	{
		GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->nativeWindow);
		glfwGetWindowSize(glfwWin, &(*width), &(*height));
	}
}
