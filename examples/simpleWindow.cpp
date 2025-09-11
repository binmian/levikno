#include <levikno/lvn_graphics.h>


int main(int argc, char** argv)
{
    lvn::initContext();

    LvnGraphicsContextCreateInfo graphicsInfo{};
    graphicsInfo.windowapi = Lvn_WindowApi_glfw;
    graphicsInfo.graphicsapi = Lvn_GraphicsApi_opengl;

    lvn::initGraphicsContext(&graphicsInfo);

    LvnWindowCreateInfo winCreateInfo = lvn::configWindowInit("simpleWindow", 800, 600);

    LvnWindow* window;
    lvn::createWindow(&window, &winCreateInfo);

    while (lvn::windowOpen(window))
    {
        lvn::windowUpdate(window);
        lvn::windowPollEvents();
    }

    lvn::destroyWindow(window);

    lvn::terminateGraphicsContext();
    lvn::terminateContext();

    return 0;
}
