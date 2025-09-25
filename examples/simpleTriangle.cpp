#include <levikno/lvn_graphics.h>


int main(int argc, char** argv)
{
    lvn::initContext();

    LvnGraphicsContextCreateInfo graphicsInfo{};
    graphicsInfo.windowapi = Lvn_WindowApi_Glfw;
    graphicsInfo.graphicsapi = Lvn_GraphicsApi_vulkan;
    graphicsInfo.enableGraphicsApiDebugLogs = true;

    lvn::initGraphicsContext(&graphicsInfo);

    LvnWindowCreateInfo windowCreateInfo = lvn::configWindowInit("simpleTriangle", 800, 600);

    LvnWindow* window;
    lvn::createWindow(&window, &windowCreateInfo);

    lvn::destroyWindow(window);

    lvn::terminateGraphicsContext();

    lvn::terminateContext();
}
