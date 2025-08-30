#include "levikno/lvn_window.h"

#include "levikno/api/lvn_glfw.h"

int main(int argc, char** argv)
{
    lvn::initLogging();

    LvnWindowContextCreateInfo winctxCreateInfo{};
    winctxCreateInfo.windowContextInitCallback = lvn::implGlfwInitWindowContext;
    winctxCreateInfo.windowContextTerminateCallback = lvn::implGlfwTerminateWindowContext;
    winctxCreateInfo.renderingBackend = Lvn_GraphicsApi_opengl;

    lvn::initWindowContext(&winctxCreateInfo);

    LvnWindowCreateInfo winCreateInfo = lvn::configWindowInit("simpleWindow", 800, 600);

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
