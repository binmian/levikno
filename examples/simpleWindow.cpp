#include <levikno/lvn_graphics.h>

#include <levikno/api/lvn_glfw.h>

int main(int argc, char** argv)
{
    lvn::initContext();

    LvnGraphicsContextCreateInfo graphicsctx{};
    graphicsctx.windowapi = Lvn_WindowApi_glfw;
    graphicsctx.graphicsapi = Lvn_GraphicsApi_opengl;
    graphicsctx.windowInitFunc = lvn::implGlfwInitWindowContext;
    graphicsctx.windowTerminateFunc = lvn::implGlfwTerminateWindowContext;
    graphicsctx.noGraphicsInit = true;
    lvn::initGraphicsContext(&graphicsctx);

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
