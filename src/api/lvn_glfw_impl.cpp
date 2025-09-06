#include "lvn_glfw_impl.h"
#include "lvn_graphics_internal.h"


#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


namespace lvn
{

static bool s_glfwInit = false;
static GLFWcursor* s_CursorIcons[10];

static void        GLFWerrorCallback(int error, const char* descripion);

static void GLFWerrorCallback(int error, const char* descripion)
{
    LVN_CORE_ERROR("[glfw]: (%d): %s", error, descripion);
}

LvnResult implGlfwInitWindowContext(LvnGraphicsContext* graphicsctx)
{
    if (s_glfwInit)
    {
        LVN_CORE_WARN("glfw already initialized!");
        return Lvn_Result_AlreadyCalled;
    }

    int success = glfwInit();
    if (!success)
    {
        LVN_ASSERT(false, "Failed to initialize glfw");
        return Lvn_Result_Failure;
    }
    s_glfwInit = true;

    graphicsctx->createWindow = glfwImplCreateWindow;
    graphicsctx->destroyWindow = glfwImplDestroyWindow;
    graphicsctx->updateWindow = glfwImplUpdateWindow;
    graphicsctx->windowOpen = glfwImplWindowOpen;
    graphicsctx->windowPollEvents = glfwImplWindowPollEvents;
    graphicsctx->getDimensions = glfwImplGetDimensions;
    graphicsctx->getWindowWidth = glfwImplGetWindowWidth;
    graphicsctx->getWindowHeight = glfwImplGetWindowHeight;
    graphicsctx->setWindowVSync = glfwImplSetWindowVSync;
    graphicsctx->getWindowVSync = glfwImplGetWindowVSync;
    graphicsctx->setWindowContextCurrent = glfwImplSetWindowContextCurrent;
    graphicsctx->keyPressed = glfwImplKeyPressed;
    graphicsctx->keyReleased = glfwImplKeyReleased;
    graphicsctx->mouseButtonPressed = glfwImplMouseButtonPressed;
    graphicsctx->mouseButtonReleased = glfwImplMouseButtonReleased;

    graphicsctx->getMousePos = glfwImplGetMousePos;
    graphicsctx->getMousePosPtr = glfwImplGetMousePosPtr;
    graphicsctx->getMouseX = glfwImplGetMouseX;
    graphicsctx->getMouseY = glfwImplGetMouseY;
    graphicsctx->setMouseCursor = glfwImplSetMouseCursor;
    graphicsctx->SetMouseInputMode = glfwImplSetMouseInputMode;

    graphicsctx->getWindowPos = glfwImplGetWindowPos;
    graphicsctx->getWindowPosPtr = glfwImplGetWindowPosPtr;
    graphicsctx->getWindowSize = glfwImplGetWindowSize;
    graphicsctx->getWindowSizePtr = glfwImplGetWindowSizePtr;


    LvnGraphicsApi graphicsapi = lvn::getGraphicsApi();

    if (graphicsapi == Lvn_GraphicsApi_vulkan || graphicsapi == Lvn_GraphicsApi_None)
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

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

void implGlfwTerminateWindowContext()
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

    // set full screen
    GLFWmonitor* fullScreen = nullptr;
    if (window->data.fullscreen)
        fullScreen = glfwGetPrimaryMonitor();

    // set window resizable
    if (window->data.resizable)
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    else
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    LvnGraphicsApi graphicsapi = lvn::getGraphicsApi();
    LvnContext* lvnctx = lvn::getContext();

    // get shared context (opengl)
    GLFWwindow* sharedContext = nullptr;
    if (graphicsapi == Lvn_GraphicsApi_opengl && lvnctx->sharedSuface != nullptr)
            sharedContext = static_cast<GLFWwindow*>(lvnctx->sharedSuface);

    // create window
    GLFWwindow* glfwWindow = glfwCreateWindow(window->data.width, window->data.height, window->data.title.c_str(), fullScreen, sharedContext);
    LVN_CORE_TRACE("[glfw] created window <GLFWwindow*> (%p): \"%s\" (w:%d,h:%d)", glfwWindow, window->data.title.c_str(), window->data.width, window->data.height);

    if (!glfwWindow)
    {
        LVN_CORE_ERROR("failed to create window: \"%s\" (w:%d, h:%d)", window->data.title.c_str(), window->data.width, window->data.height);
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

        glfwSetWindowIcon(glfwWindow, static_cast<int>(window->data.iconCount), images.data());
    }

    window->nativeWindow = glfwWindow;

    // set window size parameters & vsync
    glfwSetWindowSizeLimits(glfwWindow, window->data.minWidth, window->data.minHeight, window->data.maxWidth, window->data.maxHeight);
    glfwSetWindowUserPointer(glfwWindow, window);

    if (graphicsapi == Lvn_GraphicsApi_opengl)
    {
        glfwMakeContextCurrent(glfwWindow);
        glfwSwapInterval(createInfo->vSync);
    }

    // set GLFW Callbacks
    glfwSetWindowSizeCallback(glfwWindow, [](GLFWwindow* window, int width, int height)
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

    glfwSetFramebufferSizeCallback(glfwWindow, [](GLFWwindow* window, int width, int height)
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
                // VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(lvnWindow->apiData);
                // surfaceData->frameBufferResized = true;
            #endif
                break;
            }
            default:
            {
                break;
            }
        }
    });

    glfwSetWindowPosCallback(glfwWindow, [](GLFWwindow* window, int x, int y)
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

    glfwSetWindowFocusCallback(glfwWindow, [](GLFWwindow* window, int focused)
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

    glfwSetWindowCloseCallback(glfwWindow, [](GLFWwindow* window)
    {
        LvnWindowData* data = &((LvnWindow*)glfwGetWindowUserPointer(window))->data;
        LvnEvent event{};
        event.type = Lvn_EventType_WindowClose;
        event.category = Lvn_EventCategory_Window;
        event.handled = false;
        event.userData = data->userData;

        data->eventCallBackFn(&event);
    });

    glfwSetKeyCallback(glfwWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)
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

    glfwSetCharCallback(glfwWindow, [](GLFWwindow* window, unsigned int keycode)
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

    glfwSetMouseButtonCallback(glfwWindow, [](GLFWwindow* window, int button, int action, int mods)
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

    glfwSetScrollCallback(glfwWindow, [](GLFWwindow* window, double xOffset, double yOffset)
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

    glfwSetCursorPosCallback(glfwWindow, [](GLFWwindow* window, double xPos, double yPos)
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

void glfwImplDestroyWindow(LvnWindow* window)
{
    // destroyGraphicsRelatedAPIData(window);
    glfwDestroyWindow(static_cast<GLFWwindow*>(window->nativeWindow));
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
            // VulkanWindowSurfaceData* surfaceData = static_cast<VulkanWindowSurfaceData*>(window->apiData);
            // surfaceData->frameBufferResized = true; // set resize to true to recreate swapchain
        #endif
            break;
        }

        default: { return; }
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
        // GLFWwindow* sharedContext = static_cast<GLFWwindow*>(lvn::getMainOglWindowContext());
        // glfwMakeContextCurrent(window ? static_cast<GLFWwindow*>(window->nativeWindow) : sharedContext);

        glfwMakeContextCurrent(static_cast<GLFWwindow*>(window->nativeWindow));
    }
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
    LVN_ASSERT(static_cast<uint32_t>(cursor) < (sizeof(s_CursorIcons) / sizeof(s_CursorIcons[0])), "cursor mode index out of range");

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

} /* namespace lvn */
