#ifndef HG_LEVIKNO_GLFW_IMPL_H
#define HG_LEVIKNO_GLFW_IMPL_H

#include "lvn_graphics_internal.h"

namespace lvn
{
    LvnResult        implGlfwInitWindowContext(LvnGraphicsContext* graphicsctx);
    void             implGlfwTerminateWindowContext();

    LvnResult        implGlfwCreateWindow(LvnWindow* window, const LvnWindowCreateInfo* createInfo);
    void             implGlfwDestroyWindow(LvnWindow* window);

    void             implGlfwUpdateWindow(LvnWindow* window);
    bool             implGlfwWindowOpen(LvnWindow* window);
    void             implGlfwWindowPollEvents();
    LvnPair<int>     implGlfwGetDimensions(LvnWindow* window);
    unsigned int     implGlfwGetWindowWidth(LvnWindow* window);
    unsigned int     implGlfwGetWindowHeight(LvnWindow* window);
    void             implGlfwSetWindowVSync(LvnWindow* window, bool enable);
    bool             implGlfwGetWindowVSync(LvnWindow* window);
    void*            implGlfwGetNativeWindow(LvnWindow* window);
    void             implGlfwSetWindowContextCurrent(LvnWindow* window);
    LvnRenderPass*   implGlfwGetWindowRenderPass(LvnWindow* window);

    bool             implGlfwKeyPressed(LvnWindow* window, int keycode);
    bool             implGlfwKeyReleased(LvnWindow* window, int keycode);
    bool             implGlfwMouseButtonPressed(LvnWindow* window, int button);
    bool             implGlfwMouseButtonReleased(LvnWindow* window, int button);

    LvnPair<float>   implGlfwGetMousePos(LvnWindow* window);
    void             implGlfwGetMousePosPtr(LvnWindow* window, float* xpos, float* ypos);
    float            implGlfwGetMouseX(LvnWindow* window);
    float            implGlfwGetMouseY(LvnWindow* window);
    void             implGlfwSetMouseCursor(LvnWindow* window, LvnMouseCursor cursor);
    void             implGlfwSetMouseInputMode(LvnWindow* window, LvnMouseInputMode mode);

    LvnPair<int>     implGlfwGetWindowPos(LvnWindow* window);
    void             implGlfwGetWindowPosPtr(LvnWindow* window, int* xpos, int* ypos);
    LvnPair<int>     implGlfwGetWindowSize(LvnWindow* window);
    void             implGlfwGetWindowSizePtr(LvnWindow* window, int* width, int* height);

    LvnWindowApi     implGlfwGetNativeWindowApi();
}

#endif // !HG_CHONPS_GLFW_H
