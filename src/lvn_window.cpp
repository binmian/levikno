#include "lvn_window.h"
#include "lvn_window_internal.h"


static LvnWindowContext* s_WindowContext = nullptr;

namespace lvn
{
static const char* getWindowApiNameEnum(LvnWindowApi api);

static const char* getWindowApiNameEnum(LvnWindowApi api)
{
    switch (api)
    {
        case Lvn_WindowApi_None:  { return "None";  }
        case Lvn_WindowApi_glfw:  { return "glfw";  }
        // case Lvn_WindowApi_win32: { return "win32"; }
    }

    return "";
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
    return lvn::getWindowContext()->windowapi;
}

const char* getWindowApiName()
{
    switch (lvn::getWindowContext()->windowapi)
    {
        case Lvn_WindowApi_None:  { return "None";  }
        case Lvn_WindowApi_glfw:  { return "glfw";  }
        // case Lvn_WindowApi_win32: { return "win32"; }
    }

    lvn::logCoreError("Unknown Windows API selected!");
    return "";
}

LvnResult initWindowContext(LvnWindowContextCreateInfo* createInfo)
{
    if (s_WindowContext)
        return Lvn_Result_AlreadyCalled;

    s_WindowContext = lvn::memNew<LvnWindowContext>();
    LvnWindowContext* winctx = lvn::getWindowContext();

    // set rendering backend
    if (!createInfo->forceBackendNone && createInfo->renderingBackend == Lvn_GraphicsApi_None)
    {
        lvn::logCoreWarn("window context rendering backend was not set, the rendering backend will automatically be set to be configured with opengl");
        lvn::logCoreInfo("to force levikno to use no rendering backend, set \'forceBackendNone\' to be true");
        winctx->graphicsBackend = Lvn_GraphicsApi_opengl;
    }
    else
        winctx->graphicsBackend = createInfo->renderingBackend;

    // set window context
    if (createInfo->windowContextInitCallback == nullptr)
    {
        lvn::logCoreError("cannot create window context, init window context callback function must not be nullptr");
        return Lvn_Result_Failure;
    }
    if (createInfo->windowContextTerminateCallback == nullptr)
    {
        lvn::logCoreError("cannot create window context, terminate window context callback function must not be nullptr");
        return Lvn_Result_Failure;
    }

    winctx->implInitWindowContext = createInfo->windowContextInitCallback;
    winctx->implTerminateWindowContext = createInfo->windowContextTerminateCallback;

    if (winctx->implInitWindowContext(winctx) != Lvn_Result_Success)
    {
        lvn::logCoreError("could not create window context for: %s", lvn::getWindowApiNameEnum(winctx->windowapi));
        return Lvn_Result_Failure;
    }

    lvn::logCoreTrace("window context set: %s", lvn::getWindowApiNameEnum(winctx->windowapi));
    //windowInputInit();
    return Lvn_Result_Success;
}

void terminateWindowContext()
{
    if (!s_WindowContext)
        return;

    s_WindowContext->implTerminateWindowContext();

    lvn::logCoreTrace("window context terminated: %s", lvn::getWindowApiNameEnum(s_WindowContext->windowapi));
    lvn::memDelete<LvnWindowContext>(s_WindowContext);
}

LvnWindowContext* getWindowContext()
{
    LVN_ASSERT(s_WindowContext, "cannot get window context, window context was not created");
    return s_WindowContext;
}

LvnGraphicsApi getWindowGraphicsBackend()
{
    return lvn::getWindowContext()->graphicsBackend;
}

LvnResult createWindow(LvnWindow** window, const LvnWindowCreateInfo* createInfo)
{
    LvnWindowContext* winctx = lvn::getWindowContext();

    if (createInfo->width < 0 || createInfo->height < 0)
    {
        lvn::logCoreError("createWindow(LvnWindow**, LvnWindowCreateInfo*) | cannot create window with negative dimensions (w:%d,h:%d)", createInfo->width, createInfo->height);
        return Lvn_Result_Failure;
    }

    *window = lvn::createObject<LvnWindow>(Lvn_Stype_Window);

    lvn::logCoreTrace("created window: (%p), \"%s\" (w:%d,h:%d)", *window, createInfo->title.c_str(), createInfo->width, createInfo->height);
    return winctx->createWindow(*window, createInfo);
}

void destroyWindow(LvnWindow* window)
{
    if (window == nullptr) { return; }
    LvnWindowContext* winctx = lvn::getWindowContext();
    winctx->destroyWindow(window);
    lvn::destroyObject<LvnWindow>(window, Lvn_Stype_Window);
}

LvnWindowCreateInfo configWindowInit(const char* title, int width, int height)
{
    LvnWindowCreateInfo windowCreateInfo{};
    windowCreateInfo.width = width;
    windowCreateInfo.height = height;
    windowCreateInfo.title = title;
    windowCreateInfo.minWidth = 0;
    windowCreateInfo.minHeight = 0;
    windowCreateInfo.maxWidth = -1;
    windowCreateInfo.maxHeight = -1;
    windowCreateInfo.fullscreen = false;
    windowCreateInfo.resizable = true;
    windowCreateInfo.vSync = false;
    windowCreateInfo.pIcons = nullptr;
    windowCreateInfo.iconCount = 0;
    windowCreateInfo.eventCallBack = nullptr;
    windowCreateInfo.userData = nullptr;

    return windowCreateInfo;
}

void windowUpdate(LvnWindow* window)
{
    lvn::getWindowContext()->updateWindow(window);
}

bool windowOpen(LvnWindow* window)
{
    return lvn::getWindowContext()->windowOpen(window);
}

void windowPollEvents()
{
    lvn::getWindowContext()->windowPollEvents();
}

LvnPair<int> windowGetDimensions(LvnWindow* window)
{
    return lvn::getWindowContext()->getWindowSize(window);
}

int windowGetWidth(LvnWindow* window)
{
    return lvn::getWindowContext()->getWindowWidth(window);
}

int windowGetHeight(LvnWindow* window)
{
    return lvn::getWindowContext()->getWindowHeight(window);
}

void windowSetEventCallback(LvnWindow* window, void (*callback)(LvnEvent*), void* userData)
{
    window->data.eventCallBackFn = callback;
    window->data.userData = userData;
}

void windowSetVSync(LvnWindow* window, bool enable)
{
    lvn::getWindowContext()->setWindowVSync(window, enable);
}

bool windowGetVSync(LvnWindow* window)
{
    return lvn::getWindowContext()->getWindowVSync(window);
}

void* windowGetNativeWindow(LvnWindow* window)
{
    return window->nativeWindow;
}

LvnRenderPass* windowGetRenderPass(LvnWindow* window)
{
    return &window->renderPass;
}

void windowSetContextCurrent(LvnWindow* window)
{
    lvn::getWindowContext()->setWindowContextCurrent(window);
}

// ------------------------------------------------------------
// [SECTION]: Input Functions
// ------------------------------------------------------------

bool keyPressed(LvnWindow* window, int keycode)
{
    return lvn::getWindowContext()->keyPressed(window, keycode);
}

bool keyReleased(LvnWindow* window, int keycode)
{
    return lvn::getWindowContext()->keyReleased(window, keycode);
}

bool mouseButtonPressed(LvnWindow* window, int button)
{
    return lvn::getWindowContext()->mouseButtonPressed(window, button);
}

bool mouseButtonReleased(LvnWindow* window, int button)
{
    return lvn::getWindowContext()->mouseButtonReleased(window, button);
}

LvnPair<float> mouseGetPos(LvnWindow* window)
{
    return lvn::getWindowContext()->getMousePos(window);
}

void mouseGetPos(LvnWindow* window, float* xpos, float* ypos)
{
    lvn::getWindowContext()->getMousePosPtr(window, xpos, ypos);
}

float mouseGetX(LvnWindow* window)
{
    return lvn::getWindowContext()->getMouseX(window);
}

float mouseGetY(LvnWindow* window)
{
    return lvn::getWindowContext()->getMouseY(window);
}

void mouseSetCursor(LvnWindow* window, LvnMouseCursor cursor)
{
    lvn::getWindowContext()->setMouseCursor(window, cursor);
}

void mouseSetInputMode(LvnWindow* window, LvnMouseInputMode mode)
{
    lvn::getWindowContext()->SetMouseInputMode(window, mode);
}

LvnPair<int> windowGetPos(LvnWindow* window)
{
    return lvn::getWindowContext()->getWindowPos(window);
}

void windowGetPos(LvnWindow* window, int* xpos, int* ypos)
{
    lvn::getWindowContext()->getWindowPosPtr(window, xpos, ypos);
}

LvnPair<int> windowGetSize(LvnWindow* window)
{
    return lvn::getWindowContext()->getWindowSize(window);
}

void windowGetSize(LvnWindow* window, int* width, int* height)
{
    lvn::getWindowContext()->getWindowSizePtr(window, width, height);
}

} /* namespace lvn */
