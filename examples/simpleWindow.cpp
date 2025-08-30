#include "levikno/levikno.h"
#include "levikno/lvn_window.h"

int main(void)
{
    LvnLoggingContextCreateInfo logctxCreateInfo{};

    lvn::initLogging(&logctxCreateInfo);

    LvnWindowContextCreateInfo winctxCreateInfo{};
    winctxCreateInfo.windowapi = Lvn_WindowApi_glfw;
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
