#ifndef HG_LVN_VULKAN_H
#define HG_LVN_VULKAN_H

#include "../lvn_config.h"


struct LvnGraphicsContext;

namespace lvn
{
    LvnResult implVkInitGraphicsContext(LvnGraphicsContext* graphicsContext);
    void      implVkTerminateGraphicsContext();
}

#endif /* !HG_LVN_VULKAN_H */
