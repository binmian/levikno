#include "levikno/lvn_window.h"

#include "levikno/api/lvn_glfw.h"

int main(void)
{
    LvnLoggingContextCreateInfo logctxCreateInfo{};

    lvn::initLogging(&logctxCreateInfo);

    LvnWindowContextCreateInfo winctxCreateInfo{};
    winctxCreateInfo.windowContextInitCallback = lvn::implGlfwInitWindowContext;
    winctxCreateInfo.windowContextTerminateCallback = lvn::implGlfwTerminateWindowContext;
    winctxCreateInfo.renderingBackend = Lvn_GraphicsApi_opengl;

    lvn::initWindowContext(&winctxCreateInfo);

    LvnWindowCreateInfo createInfo = lvn::configWindowInit("simpleWindow", 800, 600);

    LvnWindow* window;
    lvn::createWindow(&window, &createInfo);

    while (lvn::windowOpen(window))
    {
        lvn::windowUpdate(window);
        lvn::windowPollEvents();
    }

    lvn::destroyWindow(window);

    lvn::terminateWindowContext();
    lvn::terminateLogging();
}
