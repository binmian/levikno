#include <levikno/lvn_renderer.h>


int main(int argc, char** argv)
{
    LvnContextCreateInfo lvnCreateInfo{};
    lvnCreateInfo.logging.enableLogging = true;
    lvnCreateInfo.logging.enableGraphicsApiDebugLogs = true;
    lvnCreateInfo.windowapi = Lvn_WindowApi_glfw;
    lvnCreateInfo.graphicsapi = Lvn_GraphicsApi_vulkan;

    lvn::createContext(&lvnCreateInfo);

    lvn::renderInit("simpleCollision", 800, 600);

    LvnWindow* window = lvn::getRendererWindow();
    lvn::windowSetVSync(window, true);

    LvnRect rect = lvn::configRectInit({100.0f, 100.0f}, {255,255,255,255});
    LvnCollisionRect collRect = {{-50.0f,-50.0f},{100.0f,100.0f}};

    LvnCollisionPoint collPoint = {0,0};

    LVN_INFO("Move the cursor into the box to detect a collision");

    while (lvn::renderWindowOpen())
    {
        lvn::windowPollEvents();

        auto[x, y] = lvn::mouseGetPos(window);
        auto[width,height] = lvn::windowGetSize(window);
        x = x - width / 2.0f;
        y = (height / 2.0f) - y;

        collPoint = {x,y};

        lvn::drawBegin();
        lvn::drawClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        if (lvn::collisionPointToRect(collPoint, collRect))
            rect.color = {255, 128, 0, 255};
        else
            rect.color = {255, 255, 255, 255};

        lvn::drawRectEx(rect, {-50.0f,-50.0f});

        lvn::drawEnd();
    }

    lvn::renderTerminate();
    lvn::terminateContext();

    return 0;
}
