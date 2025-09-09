#include <levikno/lvn_window.h>
#include <levikno/lvn_graphics.h>

#include <levikno/api/lvn_glfw.h>
#include <levikno/api/lvn_vulkan.h>


int main(int argc, char** argv)
{
    LvnContextCreateInfo ctxinfo{};
    ctxinfo.logging.enableGraphicsApiDebugLogs = true;
    lvn::initContext(&ctxinfo);

    LvnWindowContextCreateInfo winctx{};
    winctx.windowContextInitFunc = lvn::implGlfwInitWindowContext;
    winctx.windowContextTerminateFunc = lvn::implGlfwTerminateWindowContext;
    winctx.renderingBackend = Lvn_GraphicsApi_vulkan;
    lvn::initWindowContext(&winctx);

    LvnGraphicsContextCreateInfo graphicsctx{};
    graphicsctx.graphicsContextInitFunc = lvn::implVkInitGraphicsContext;
    graphicsctx.graphicsContextTerminateFunc = lvn::implVkTerminateGraphicsContext;
    lvn::initGraphicsContext(&graphicsctx);



    lvn::terminateGraphicsContext();
    lvn::terminateWindowContext();

    lvn::terminateContext();
}
