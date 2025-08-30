#include "levikno/lvn_window.h"

#include "levikno/api/lvn_glfw.h"

bool keyHold(LvnKeyHoldEvent* event, void* userData)
{
    lvn::logInfo("%s, key: %d, repeat: %s", event->name, event->keyCode, event->repeat ? "true" : "false");
    return true;
}

bool keyPressed(LvnKeyPressedEvent* event, void* userData)
{
    lvn::logInfo("%s, key: %d", event->name, event->keyCode);
    return true;
}

bool keyReleased(LvnKeyReleasedEvent* event, void* userData)
{
    lvn::logInfo("%s, key: %d", event->name, event->keyCode);
    return true;
}

bool keyTyped(LvnKeyTypedEvent* event, void* userData)
{
    lvn::logInfo("%s, key: %c", event->name, event->key);
    return true;
}

bool mouseButtonPressed(LvnMouseButtonPressedEvent* event, void* userData)
{
    lvn::logInfo("%s, button: %u", event->name, event->buttonCode);
    return true;
}

bool mouseButtonReleased(LvnMouseButtonReleasedEvent* event, void* userData)
{
    lvn::logInfo("%s, button: %u", event->name, event->buttonCode);
    return true;
}

bool mouseMovedEvent(LvnMouseMovedEvent* event, void* userData)
{
    lvn::logInfo("%s, (x:%d,y:%d)", event->name, event->x, event->y);
    return true;
}

bool mouseScrolledEvent(LvnMouseScrolledEvent* event, void* userData)
{
    lvn::logInfo("%s, (x:%.2f,y:%.2f)", event->name, event->x, event->y);
    return true;
}

bool windowClosedEvent(LvnWindowCloseEvent* event, void* userData)
{
    lvn::logInfo("%s", event->name);
    return true;
}

bool windowFrameBufferResizedEvent(LvnWindowFramebufferResizeEvent* event, void* userData)
{
    lvn::logInfo("%s, (w:%u,h:%u)", event->name, event->width, event->height);
    return true;
}

bool windowFocusedEvent(LvnWindowFocusEvent* event, void* userData)
{
    lvn::logInfo("%s", event->name);
    return true;
}

bool windowLostFocusedEvent(LvnWindowLostFocusEvent* event, void* userData)
{
    lvn::logInfo("%s", event->name);
    return true;
}

bool windowMovedEvent(LvnWindowMovedEvent* event, void* userData)
{
    lvn::logInfo("%s, (x:%d,y:%d)", event->name, event->x, event->y);
    return true;
}

bool windowResizeEvent(LvnWindowResizeEvent* event, void* userData)
{
    lvn::logInfo("%s, (x:%d,y:%d)", event->name, event->width, event->height);
    return true;
}

void eventCallBackFn(LvnEvent* event)
{
    lvn::dispatchKeyHoldEvent(event, keyHold);
    lvn::dispatchKeyPressedEvent(event, keyPressed);
    lvn::dispatchKeyReleasedEvent(event, keyReleased);
    lvn::dispatchKeyTypedEvent(event, keyTyped);
    lvn::dispatchMouseButtonPressedEvent(event, mouseButtonPressed);
    lvn::dispatchMouseButtonReleasedEvent(event, mouseButtonReleased);
    lvn::dispatchMouseMovedEvent(event, mouseMovedEvent);
    lvn::dispatchMouseScrolledEvent(event, mouseScrolledEvent);
    lvn::dispatchWindowCloseEvent(event, windowClosedEvent);
    lvn::dispatchWindowFramebufferResizeEvent(event, windowFrameBufferResizedEvent);
    lvn::dispatchWindowFocusEvent(event, windowFocusedEvent);
    lvn::dispatchWindowLostFocusEvent(event, windowLostFocusedEvent);
    lvn::dispatchWindowMovedEvent(event, windowMovedEvent);
    lvn::dispatchWindowResizeEvent(event, windowResizeEvent);
}

int main(int argc, char** argv)
{
    lvn::initLogging();

    LvnWindowContextCreateInfo winctxCreateInfo{};
    winctxCreateInfo.windowContextInitCallback = lvn::implGlfwInitWindowContext;
    winctxCreateInfo.windowContextTerminateCallback = lvn::implGlfwTerminateWindowContext;
    winctxCreateInfo.renderingBackend = Lvn_GraphicsApi_opengl;

    lvn::initWindowContext(&winctxCreateInfo);

    LvnWindowCreateInfo winCreateInfo = lvn::configWindowInit("events", 800, 600);
    winCreateInfo.eventCallBack = eventCallBackFn; // set the event call back function to record callbacks

    LvnWindow* window;
    lvn::createWindow(&window, &winCreateInfo);

    while (lvn::windowOpen(window))
    {
        lvn::windowUpdate(window);
        lvn::windowPollEvents();
    }

    lvn::destroyWindow(window);

    lvn::terminateWindowContext();
    lvn::terminateLogging();

    return 0;
}
