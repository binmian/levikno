#ifndef HG_LVN_GLFW_H
#define HG_LVN_GLFW_H

#include "../lvn_config.h"


struct LvnWindowContext;

namespace lvn
{
    LvnResult implGlfwInitWindowContext(LvnWindowContext* windowContext);
    void      implGlfwTerminateWindowContext();
}

#endif /* !HG_LVN_GLFW_H */
