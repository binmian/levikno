#include <levikno/lvn_graphics.h>

#include <levikno/api/lvn_glfw.h>
#include <levikno/api/lvn_vulkan.h>


int main(int argc, char** argv)
{
    lvn::initContext();

    LvnGraphicsContextCreateInfo graphicsctx{};
    graphicsctx.windowapi = Lvn_WindowApi_glfw;
    graphicsctx.graphicsapi = Lvn_GraphicsApi_vulkan;
    graphicsctx.windowInitFunc = lvn::implGlfwInitWindowContext;
    graphicsctx.windowTerminateFunc = lvn::implGlfwTerminateWindowContext;
    graphicsctx.graphicsInitFunc = lvn::implVkInitGraphicsContext;
    graphicsctx.graphicsTerminateFunc = lvn::implVkTerminateGraphicsContext;
    graphicsctx.enableGraphicsApiDebugLogs = true;
    lvn::initGraphicsContext(&graphicsctx);



    lvn::terminateGraphicsContext();

    lvn::terminateContext();
}
