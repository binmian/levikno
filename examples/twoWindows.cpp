#include <levikno/levikno.h>


// NOTE: this example shows how to create and run to windows on different threads


static int s_WindowCount = 0;

void renderWindow(LvnWindow* window)
{
    // NOTE: the window's context must be made current on the thread it will be used on
    lvn::windowSetContextCurrent(window);

    // [Main Render Loop]
    while (lvn::windowOpen(window))
    {
        lvn::windowUpdate(window);

        lvn::renderBeginNextFrame(window);
        lvn::renderBeginCommandRecording(window);
        lvn::renderCmdBeginRenderPass(window, 0.0f, 0.0f, 0.0f, 1.0f);

        lvn::renderCmdEndRenderPass(window);
        lvn::renderEndCommandRecording(window);
        lvn::renderDrawSubmit(window);
    }

    lvn::destroyWindow(window);
    s_WindowCount--;
}

int main(int argc, char** argv)
{
    LvnContextCreateInfo lvnCreateInfo{};
    lvnCreateInfo.logging.enableLogging = true;
    lvnCreateInfo.windowapi = Lvn_WindowApi_glfw;
    lvnCreateInfo.graphicsapi = Lvn_GraphicsApi_vulkan;

    lvn::createContext(&lvnCreateInfo);

    // window create info struct
    LvnWindowCreateInfo windowInfo{};
    windowInfo.title = "window1";
    windowInfo.width = 800;
    windowInfo.height = 600;
    windowInfo.minWidth = 300;
    windowInfo.minHeight = 200;

    // NOTE: windows need to be created on the same thread that the context was initialized on
    LvnWindow* window1;
    lvn::createWindow(&window1, &windowInfo);

    windowInfo.title = "window2";

    LvnWindow* window2;
    lvn::createWindow(&window2, &windowInfo);

    // NOTE: set the main thread window context to nullptr if no windows are used on it
    lvn::windowSetContextCurrent(nullptr);

    s_WindowCount = 2;
    std::thread t1(renderWindow, window1);
    std::thread t2(renderWindow, window2);

    // NOTE: calling the windowOpen function is not thread safe in case a window is destroyed while checking if a window is open 
    // instead we check the number of windows currently open for this example
    while (s_WindowCount)
    {
        lvn::windowPollEvents();
    }

    t1.join();
    t2.join();

    lvn::terminateContext();

    return 0;
}
