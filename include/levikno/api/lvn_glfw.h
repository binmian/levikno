#ifndef HG_LVN_GLFW_H
#define HG_LVN_GLFW_H


enum LvnResult : int;
struct LvnGraphicsContext;

namespace lvn
{
    LvnResult implGlfwInitWindowContext(LvnGraphicsContext* windowContext);
    void      implGlfwTerminateWindowContext();
}

#endif /* !HG_LVN_GLFW_H */
