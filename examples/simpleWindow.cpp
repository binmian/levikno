#include <levikno/lvn_window.h>

#include <levikno/api/lvn_glfw.h>

int main(int argc, char** argv)
{
    lvn::initContext();

    LvnWindowContextCreateInfo winctxCreateInfo{};
    winctxCreateInfo.windowContextInitFunc = lvn::implGlfwInitWindowContext;
    winctxCreateInfo.windowContextTerminateFunc = lvn::implGlfwTerminateWindowContext;
    winctxCreateInfo.renderingBackend = Lvn_GraphicsApi_opengl;
    lvn::initWindowContext(&winctxCreateInfo);

    LvnWindowCreateInfo winCreateInfo = lvn::configWindowInit("simpleWindow", 800, 600);

    LvnWindow* window;
    lvn::createWindow(&window, &winCreateInfo);


    lvn::destroyWindow(window);

    lvn::terminateWindowContext();
    lvn::terminateContext();

    return 0;
}
