#include <levikno/levikno.h>


int main(int argc, char** argv)
{
    LvnContextCreateInfo lvnCreateInfo{};
    lvnCreateInfo.logging.enableLogging = true;
    lvnCreateInfo.logging.enableGraphicsApiDebugLogs = true;
    lvnCreateInfo.windowapi = Lvn_WindowApi_glfw;
    lvnCreateInfo.graphicsapi = Lvn_GraphicsApi_opengl;

    lvn::createContext(&lvnCreateInfo);

    lvn::renderInit("basicUsage", 800, 600);

    LvnWindow* window = lvn::getRendererWindow();
    lvn::windowSetVSync(window, true);

    while (lvn::renderWindowOpen())
    {
        lvn::windowPollEvents();

        lvn::drawBegin();
        lvn::drawClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        lvn::drawRect({0.0f, 0.0f}, {50.0f, 50.0f}, {255,255,255,255});
        lvn::drawRect({0.0f, 100.0f}, {128.0f, 128.0f}, {255,0,127,255});
        lvn::drawRect({cos(lvn::getContextTime()) * 200.0f, sin(lvn::getContextTime()) * 200.0f}, {20.0f, 80.0f}, {127,127,255,255});

        lvn::drawTriangle({-400.0f, 0.0f}, {-350.0f, 50.0f}, {-300.0f, 0.0f}, {255,255,0,255});
        lvn::drawTriangle({-400.0f, -100.0f}, {-350.0f, -50.0f}, {-300.0f, sin(lvn::getContextTime() * 2) * 25.0f - 75.0f}, {255,0,255,255});

        lvn::drawCircle({-400.0f, 400.0f}, 200.0f, {0, 255, 255});
        lvn::drawCircleSector({-400.0f, 400.0f}, 150.0f, 90.0f, 315.0f, {20, 20, 128});
        lvn::drawPolyNgon({-400.0f, 400.0f}, 100.0f, 6, {100, 0, 20});
        lvn::drawPolyNgonSector({-400.0f, 400.0f}, 50.0f, 0.0f, 145.0f, 3, {0, 0, 0});

        lvn::drawText("hello world", {200.0f, -300.0f}, {225,255,255}, 1.0f);
        lvn::drawText("giga\nchad", {200.0f + 50 * sin(lvn::getContextTime()), -400.0f}, {225,255,255}, 2.0f);
        lvn::drawTextEx("The quick brown fox jumps over the lazy dog", {0.0f, 450.0f}, {225,255,255}, 1.0f, 2.0f, 200.0f + 100 * cos(lvn::getContextTime()));

        lvn::drawEnd();
    }

    lvn::terminateContext();

    return 0;
}
