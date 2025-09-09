#ifndef HG_LVN_GLFW_H
#define HG_LVN_GLFW_H

#include "../lvn_config.h"


struct LvnGraphicsContext;

namespace lvn
{
    LvnResult implGlfwInitWindowContext(LvnGraphicsContext* windowContext);
    void      implGlfwTerminateWindowContext();
}

#endif /* !HG_LVN_GLFW_H */
