#ifndef HG_LEVIKNO_GLFW_IMPL_H
#define HG_LEVIKNO_GLFW_IMPL_H

#include "lvn_graphics_internal.h"

namespace lvn
{
    LvnResult implGlfwInitWindowContext(LvnGraphicsContext* graphicsctx);
    void implGlfwTerminateWindowContext();

    LvnResult glfwImplCreateWindow(LvnWindow* window, const LvnWindowCreateInfo* createInfo);
    void glfwImplDestroyWindow(LvnWindow* window);

    void glfwImplUpdateWindow(LvnWindow* window);
    bool glfwImplWindowOpen(LvnWindow* window);
    void glfwImplWindowPollEvents();
    LvnPair<int> glfwImplGetDimensions(LvnWindow* window);
    unsigned int glfwImplGetWindowWidth(LvnWindow* window);
    unsigned int glfwImplGetWindowHeight(LvnWindow* window);
    void glfwImplSetWindowVSync(LvnWindow* window, bool enable);
    bool glfwImplGetWindowVSync(LvnWindow* window);
    void* glfwImplGetNativeWindow(LvnWindow* window);
    void glfwImplSetWindowContextCurrent(LvnWindow* window);

    bool glfwImplKeyPressed(LvnWindow* window, int keycode);
    bool glfwImplKeyReleased(LvnWindow* window, int keycode);
    bool glfwImplMouseButtonPressed(LvnWindow* window, int button);
    bool glfwImplMouseButtonReleased(LvnWindow* window, int button);

    LvnPair<float> glfwImplGetMousePos(LvnWindow* window);
    void glfwImplGetMousePosPtr(LvnWindow* window, float* xpos, float* ypos);
    float glfwImplGetMouseX(LvnWindow* window);
    float glfwImplGetMouseY(LvnWindow* window);
    void glfwImplSetMouseCursor(LvnWindow* window, LvnMouseCursor cursor);
    void glfwImplSetMouseInputMode(LvnWindow* window, LvnMouseInputMode mode);

    LvnPair<int> glfwImplGetWindowPos(LvnWindow* window);
    void glfwImplGetWindowPosPtr(LvnWindow* window, int* xpos, int* ypos);
    LvnPair<int> glfwImplGetWindowSize(LvnWindow* window);
    void glfwImplGetWindowSizePtr(LvnWindow* window, int* width, int* height);

    LvnWindowApi glfwImplGetNativeWindowApi();
}

#endif // !HG_CHONPS_GLFW_H
