#include "lvn_window.h"
#include "lvn_window_internal.h"


#ifdef LVN_INCLUDE_GLFW
#   include "lvn_impl_glfw.h"
#endif

#ifdef LVN_INCLUDE_WAYLAND
#   include "lvn_impl_wl.h"
#endif


static LvnWindowContext* s_WindowContext = nullptr;

namespace lvn
{

static LvnResult initWindowApiFuncs(LvnWindowContext* ctx);
static void terminateWindowApiFuncs(LvnWindowContext* ctx);

static LvnResult initWindowApiFuncs(LvnWindowContext* ctx)
{
    LvnResult result = Lvn_Result_Failure;
    switch (ctx->windowapi)
    {
        case Lvn_WindowApi_None:
        {
            LVN_CORE_TRACE("no window api selected, window related functions will not be set");
            return Lvn_Result_Success;
        }
        case Lvn_WindowApi_Glfw:
        {
#ifdef LVN_INCLUDE_GLFW
            result = lvn::implGlfwInitWindowContext(ctx);
#endif /* !LVN_INCLUDE_GLFW */
            break;
        }
        case Lvn_WindowApi_Wayland:
        {
#ifdef LVN_INCLUDE_WAYLAND
            result = lvn::implWaylandInitWindowContext(ctx);
#endif /* !LVN_INCLUDE_WAYLAND */
            break;
        }

        default:
        {
            LVN_CORE_ERROR("unrecognized window api: (%d), cannot create window api related functions", ctx->windowapi);
            return Lvn_Result_Failure;
        }
    }

    if (result != Lvn_Result_Success)
    {
        LVN_CORE_ERROR("could not create window api related functions for: %s", lvn::getWindowApiNameEnum(ctx->windowapi));
        return Lvn_Result_Failure;
    }

    LVN_CORE_TRACE("window api set: %s", lvn::getWindowApiNameEnum(ctx->windowapi));
    return result;
}

static void terminateWindowApiFuncs(LvnWindowContext* ctx)
{
    switch (ctx->windowapi)
    {
        case Lvn_WindowApi_None: { break; }
        case Lvn_WindowApi_Glfw:
        {
#ifdef LVN_INCLUDE_GLFW
            lvn::implGlfwTerminateWindowContext();
#endif /* !LVN_INCLUDE_GLFW */
            break;
        }
        case Lvn_WindowApi_Wayland:
        {
#ifdef LVN_INCLUDE_WAYLAND
            lvn::implWaylandTerminateWindowContext();
#endif /* !LVN_INCLUDE_WAYLAND */
            break;
        }

        default:
        {
            LVN_CORE_ERROR("unrecognized window api: (%d), cannot terminate window api related functions", ctx->windowapi);
            return;
        }
    }

    LVN_CORE_TRACE("window api terminated: %s", lvn::getWindowApiNameEnum(ctx->windowapi));
}


const char* getWindowApiNameEnum(LvnWindowApi api)
{
    switch (api)
    {
        case Lvn_WindowApi_None:    { return "None"; }
        case Lvn_WindowApi_Glfw:    { return "glfw";  }
        case Lvn_WindowApi_Win32:   { return "win32"; }
        case Lvn_WindowApi_Cocoa:   { return "cocoa"; }
        case Lvn_WindowApi_Wayland: { return "wayland"; }
        case Lvn_WindowApi_X11:     { return "x11"; }
    }

    LVN_CORE_ERROR("unknown graphics api selected");
    return "";
}

LvnResult initWindowContext(LvnWindowContextCreateInfo* createInfo)
{
    if (s_WindowContext)
        return Lvn_Result_AlreadyCalled;

    s_WindowContext = lvn::memNew<LvnWindowContext>();
    LvnWindowContext* winctx = lvn::getWindowContext();
    winctx->windowapi = createInfo->windowapi;

    if (lvn::initWindowApiFuncs(winctx) != Lvn_Result_Success)
    {
        lvn::memDelete<LvnWindowContext>(s_WindowContext);
        s_WindowContext = nullptr;
        return Lvn_Result_Failure;
    }

    return Lvn_Result_Success;
}

void terminateWindowContext()
{
    if (!s_WindowContext)
        return;

    lvn::terminateWindowApiFuncs(s_WindowContext);
    lvn::memDelete<LvnWindowContext>(s_WindowContext);
}

// -- event functions
bool dispatchKeyHoldEvent(LvnEvent* event, bool(*func)(LvnKeyHoldEvent*, void*))
{
    if (event->type == Lvn_EventType_KeyHold)
    {
        LvnKeyHoldEvent eventType{};
        eventType.type = Lvn_EventType_KeyHold;
        eventType.category = Lvn_EventCategory_Input | Lvn_EventCategory_Keyboard;
        eventType.name = "LvnKeyHoldEvent";
        eventType.handled = false;
        eventType.keyCode = event->data.code;
        eventType.repeat = event->data.repeat;

        return func(&eventType, event->userData);
    }

    return false;
}
bool dispatchKeyPressedEvent(LvnEvent* event, bool(*func)(LvnKeyPressedEvent*, void*))
{
    if (event->type == Lvn_EventType_KeyPressed)
    {
        LvnKeyPressedEvent eventType{};
        eventType.type = Lvn_EventType_KeyPressed;
        eventType.category = Lvn_EventCategory_Input | Lvn_EventCategory_Keyboard;
        eventType.name = "LvnKeyPressedEvent";
        eventType.handled = false;
        eventType.keyCode = event->data.code;

        return func(&eventType, event->userData);
    }

    return false;
}
bool dispatchKeyReleasedEvent(LvnEvent* event, bool(*func)(LvnKeyReleasedEvent*, void*))
{
    if (event->type == Lvn_EventType_KeyReleased)
    {
        LvnKeyReleasedEvent eventType{};
        eventType.type = Lvn_EventType_KeyReleased;
        eventType.category = Lvn_EventCategory_Input | Lvn_EventCategory_Keyboard;
        eventType.name = "LvnKeyReleasedEvent";
        eventType.handled = false;
        eventType.keyCode = event->data.code;

        return func(&eventType, event->userData);
    }

    return false;
}
bool dispatchKeyTypedEvent(LvnEvent* event, bool(*func)(LvnKeyTypedEvent*, void*))
{
    if (event->type == Lvn_EventType_KeyTyped)
    {
        LvnKeyTypedEvent eventType{};
        eventType.type = Lvn_EventType_KeyTyped;
        eventType.category = Lvn_EventCategory_Input | Lvn_EventCategory_Keyboard;
        eventType.name = "LvnKeyTypedEvent";
        eventType.handled = false;
        eventType.key = event->data.ucode;

        return func(&eventType, event->userData);
    }

    return false;
}
bool dispatchMouseButtonPressedEvent(LvnEvent* event, bool(*func)(LvnMouseButtonPressedEvent*, void*))
{
    if (event->type == Lvn_EventType_MouseButtonPressed)
    {
        LvnMouseButtonPressedEvent eventType{};
        eventType.type = Lvn_EventType_MouseButtonPressed;
        eventType.category = Lvn_EventCategory_Input | Lvn_EventCategory_MouseButton | Lvn_EventCategory_Mouse;
        eventType.name = "LvnMouseButtonPressedEvent";
        eventType.handled = false;
        eventType.buttonCode = event->data.code;

        return func(&eventType, event->userData);
    }

    return false;
}
bool dispatchMouseButtonReleasedEvent(LvnEvent* event, bool(*func)(LvnMouseButtonReleasedEvent*, void*))
{
    if (event->type == Lvn_EventType_MouseButtonReleased)
    {
        LvnMouseButtonReleasedEvent eventType{};
        eventType.type = Lvn_EventType_MouseButtonReleased;
        eventType.category = Lvn_EventCategory_Input | Lvn_EventCategory_MouseButton | Lvn_EventCategory_Mouse;
        eventType.name = "LvnMouseButtonReleasedEvent";
        eventType.handled = false;
        eventType.buttonCode = event->data.code;

        return func(&eventType, event->userData);
    }

    return false;
}
bool dispatchMouseMovedEvent(LvnEvent* event, bool(*func)(LvnMouseMovedEvent*, void*))
{
    if (event->type == Lvn_EventType_MouseMoved)
    {
        LvnMouseMovedEvent eventType{};
        eventType.type = Lvn_EventType_MouseMoved;
        eventType.category = Lvn_EventCategory_Input | Lvn_EventCategory_Mouse;
        eventType.name = "LvnMouseMovedEvent";
        eventType.handled = false;
        eventType.x = event->data.xd;
        eventType.y = event->data.yd;

        return func(&eventType, event->userData);
    }

    return false;
}
bool dispatchMouseScrolledEvent(LvnEvent* event, bool(*func)(LvnMouseScrolledEvent*, void*))
{
    if (event->type == Lvn_EventType_MouseScrolled)
    {
        LvnMouseScrolledEvent eventType{};
        eventType.type = Lvn_EventType_MouseScrolled;
        eventType.category = Lvn_EventCategory_Input | Lvn_EventCategory_MouseButton | Lvn_EventCategory_Mouse;
        eventType.name = "LvnMouseScrolledEvent";
        eventType.handled = false;
        eventType.x = static_cast<float>(event->data.xd);
        eventType.y = static_cast<float>(event->data.yd);

        return func(&eventType, event->userData);
    }

    return false;
}
bool dispatchWindowCloseEvent(LvnEvent* event, bool(*func)(LvnWindowCloseEvent*, void*))
{
    if (event->type == Lvn_EventType_WindowClose)
    {
        LvnWindowCloseEvent eventType{};
        eventType.type = Lvn_EventType_WindowClose;
        eventType.category = Lvn_EventCategory_Window;
        eventType.name = "LvnWindowCloseEvent";
        eventType.handled = false;

        return func(&eventType, event->userData);
    }

    return false;
}
bool dispatchWindowFocusEvent(LvnEvent* event, bool(*func)(LvnWindowFocusEvent*, void*))
{
    if (event->type == Lvn_EventType_WindowFocus)
    {
        LvnWindowFocusEvent eventType{};
        eventType.type = Lvn_EventType_WindowFocus;
        eventType.category = Lvn_EventCategory_Window;
        eventType.name = "LvnWindowFocusEvent";
        eventType.handled = false;

        return func(&eventType, event->userData);
    }

    return false;
}
bool dispatchWindowFramebufferResizeEvent(LvnEvent* event, bool(*func)(LvnWindowFramebufferResizeEvent*, void*))
{
    if (event->type == Lvn_EventType_WindowFramebufferResize)
    {
        LvnWindowFramebufferResizeEvent eventType{};
        eventType.type = Lvn_EventType_WindowFramebufferResize;
        eventType.category = Lvn_EventCategory_Window;
        eventType.name = "LvnWindowFramebufferResizeEvent";
        eventType.handled = false;
        eventType.width = event->data.x;
        eventType.height = event->data.y;

        return func(&eventType, event->userData);
    }

    return false;
}
bool dispatchWindowLostFocusEvent(LvnEvent* event, bool(*func)(LvnWindowLostFocusEvent*, void*))
{
    if (event->type == Lvn_EventType_WindowLostFocus)
    {
        LvnWindowLostFocusEvent eventType{};
        eventType.type = Lvn_EventType_WindowLostFocus;
        eventType.category = Lvn_EventCategory_Window;
        eventType.name = "LvnWindowLostFocusEvent";
        eventType.handled = false;

        return func(&eventType, event->userData);
    }

    return false;
}
bool dispatchWindowMovedEvent(LvnEvent* event, bool(*func)(LvnWindowMovedEvent*, void*))
{
    if (event->type == Lvn_EventType_WindowMoved)
    {
        LvnWindowMovedEvent eventType{};
        eventType.type = Lvn_EventType_WindowMoved;
        eventType.category = Lvn_EventCategory_Window;
        eventType.name = "LvnWindowMovedEvent";
        eventType.handled = false;
        eventType.x = event->data.x;
        eventType.y = event->data.y;

        return func(&eventType, event->userData);
    }

    return false;
}
bool dispatchWindowResizeEvent(LvnEvent* event, bool(*func)(LvnWindowResizeEvent*, void*))
{

    if (event->type == Lvn_EventType_WindowResize)
    {
        LvnWindowResizeEvent eventType{};
        eventType.type = Lvn_EventType_WindowResize;
        eventType.category = Lvn_EventCategory_Window;
        eventType.name = "LvnWindowResizeEvent";
        eventType.handled = false;
        eventType.width = event->data.x;
        eventType.height = event->data.y;

        return func(&eventType, event->userData);
    }

    return false;
}

// -- window functions
LvnWindowApi getWindowApi()
{
    return lvn::getGraphicsContext()->windowapi;
}

const char* getWindowApiName()
{
    switch (lvn::getGraphicsContext()->windowapi)
    {
        case Lvn_WindowApi_None:    { return "None";  }
        case Lvn_WindowApi_Glfw:    { return "glfw";  }
        case Lvn_WindowApi_Win32:   { return "win32"; }
        case Lvn_WindowApi_Cocoa:   { return "cocoa"; }
        case Lvn_WindowApi_Wayland: { return "wayland"; }
        case Lvn_WindowApi_X11:     { return "x11"; }
    }

    LVN_CORE_ERROR("unknown window api selected");
    return "";
}

LvnWindowApi getNativeWindowApi()
{
    return lvn::getGraphicsContext()->getNativeWindowApi();
}

LvnResult createWindow(LvnWindow** window, const LvnWindowCreateInfo* createInfo)
{
    LvnGraphicsContext* graphicsctx = lvn::getGraphicsContext();

    if (createInfo->width < 0 || createInfo->height < 0)
    {
        LVN_CORE_ERROR("createWindow(LvnWindow**, LvnWindowCreateInfo*) | cannot create window with negative dimensions (w:%d,h:%d)", createInfo->width, createInfo->height);
        return Lvn_Result_Failure;
    }

    *window = lvn::createObject<LvnWindow>(Lvn_Stype_Window);
    LvnWindow* windowPtr = *window;

    windowPtr->width = createInfo->width;
    windowPtr->height = createInfo->height;
    windowPtr->title = createInfo->title;
    windowPtr->minWidth = createInfo->minWidth;
    windowPtr->minHeight = createInfo->minHeight;
    windowPtr->maxWidth = createInfo->maxWidth;
    windowPtr->maxHeight = createInfo->maxHeight;
    windowPtr->fullscreen = createInfo->fullscreen;
    windowPtr->resizable = createInfo->resizable;
    windowPtr->vSync = createInfo->vSync;
    windowPtr->icons = lvn::move(LvnVector<LvnImage>(createInfo->pIcons, createInfo->iconCount));
    windowPtr->windowOpen = true;

    if (createInfo->eventCallBack == nullptr)
        windowPtr->eventCallBackFn = [](LvnEvent*) -> void { return; };
    else
        windowPtr->eventCallBackFn = createInfo->eventCallBack;

    windowPtr->userData = createInfo->userData;


    LVN_CORE_TRACE("created window: (%p), \"%s\" (w:%d,h:%d)", *window, createInfo->title.c_str(), createInfo->width, createInfo->height);
    return graphicsctx->createWindow(*window, createInfo);
}

void destroyWindow(LvnWindow* window)
{
    if (window == nullptr) { return; }
    LvnGraphicsContext* graphicsctx = lvn::getGraphicsContext();
    graphicsctx->destroyWindow(window);
    lvn::destroyObject<LvnWindow>(window, Lvn_Stype_Window);
}

LvnWindowCreateInfo configWindowInit(const char* title, int width, int height)
{
    LvnWindowCreateInfo createInfo{};
    createInfo.width = width;
    createInfo.height = height;
    createInfo.title = title;
    createInfo.minWidth = 0;
    createInfo.minHeight = 0;
    createInfo.maxWidth = -1;
    createInfo.maxHeight = -1;
    createInfo.fullscreen = false;
    createInfo.resizable = true;
    createInfo.vSync = false;
    createInfo.pIcons = nullptr;
    createInfo.iconCount = 0;
    createInfo.eventCallBack = nullptr;
    createInfo.userData = nullptr;

    return createInfo;
}

void windowUpdate(LvnWindow* window)
{
    lvn::getGraphicsContext()->updateWindow(window);
}

bool windowOpen(LvnWindow* window)
{
    return lvn::getGraphicsContext()->windowOpen(window);
}

void windowPollEvents()
{
    lvn::getGraphicsContext()->windowPollEvents();
}

LvnPair<int> windowGetDimensions(LvnWindow* window)
{
    return lvn::getGraphicsContext()->getWindowSize(window);
}

int windowGetWidth(LvnWindow* window)
{
    return lvn::getGraphicsContext()->getWindowWidth(window);
}

int windowGetHeight(LvnWindow* window)
{
    return lvn::getGraphicsContext()->getWindowHeight(window);
}

void windowSetEventCallback(LvnWindow* window, void (*callback)(LvnEvent*), void* userData)
{
    window->eventCallBackFn = callback;
    window->userData = userData;
}

void windowSetVSync(LvnWindow* window, bool enable)
{
    lvn::getGraphicsContext()->setWindowVSync(window, enable);
}

bool windowGetVSync(LvnWindow* window)
{
    return lvn::getGraphicsContext()->getWindowVSync(window);
}

void* windowGetNativeWindow(LvnWindow* window)
{
    return lvn::getGraphicsContext()->getNativeWindow(window);
}

LvnRenderPass* windowGetRenderPass(LvnWindow* window)
{
    return lvn::getGraphicsContext()->getWindowRenderPass(window);
}

void windowSetContextCurrent(LvnWindow* window)
{
    lvn::getGraphicsContext()->setWindowContextCurrent(window);
}

// ------------------------------------------------------------
// [SECTION]: Input Functions
// ------------------------------------------------------------

bool keyPressed(LvnWindow* window, int keycode)
{
    return lvn::getGraphicsContext()->keyPressed(window, keycode);
}

bool keyReleased(LvnWindow* window, int keycode)
{
    return lvn::getGraphicsContext()->keyReleased(window, keycode);
}

bool mouseButtonPressed(LvnWindow* window, int button)
{
    return lvn::getGraphicsContext()->mouseButtonPressed(window, button);
}

bool mouseButtonReleased(LvnWindow* window, int button)
{
    return lvn::getGraphicsContext()->mouseButtonReleased(window, button);
}

LvnPair<float> mouseGetPos(LvnWindow* window)
{
    return lvn::getGraphicsContext()->getMousePos(window);
}

void mouseGetPos(LvnWindow* window, float* xpos, float* ypos)
{
    lvn::getGraphicsContext()->getMousePosPtr(window, xpos, ypos);
}

float mouseGetX(LvnWindow* window)
{
    return lvn::getGraphicsContext()->getMouseX(window);
}

float mouseGetY(LvnWindow* window)
{
    return lvn::getGraphicsContext()->getMouseY(window);
}

void mouseSetCursor(LvnWindow* window, LvnMouseCursor cursor)
{
    lvn::getGraphicsContext()->setMouseCursor(window, cursor);
}

void mouseSetInputMode(LvnWindow* window, LvnMouseInputMode mode)
{
    lvn::getGraphicsContext()->SetMouseInputMode(window, mode);
}

LvnPair<int> windowGetPos(LvnWindow* window)
{
    return lvn::getGraphicsContext()->getWindowPos(window);
}

void windowGetPos(LvnWindow* window, int* xpos, int* ypos)
{
    lvn::getGraphicsContext()->getWindowPosPtr(window, xpos, ypos);
}

LvnPair<int> windowGetSize(LvnWindow* window)
{
    return lvn::getGraphicsContext()->getWindowSize(window);
}

void windowGetSize(LvnWindow* window, int* width, int* height)
{
    lvn::getGraphicsContext()->getWindowSizePtr(window, width, height);
}

} /* namespace lvn */
