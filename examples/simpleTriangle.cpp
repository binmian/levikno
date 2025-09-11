#include <levikno/lvn_graphics.h>


int main(int argc, char** argv)
{
    lvn::initContext();

    LvnGraphicsContextCreateInfo graphicsInfo{};
    graphicsInfo.windowapi = Lvn_WindowApi_glfw;
    graphicsInfo.graphicsapi = Lvn_GraphicsApi_opengl;
    graphicsInfo.enableGraphicsApiDebugLogs = true;

    lvn::initGraphicsContext(&graphicsInfo);


    lvn::terminateGraphicsContext();

    lvn::terminateContext();
}
