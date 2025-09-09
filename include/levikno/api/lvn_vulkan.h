#ifndef HG_LVN_VULKAN_H
#define HG_LVN_VULKAN_H


enum LvnResult : int;
struct LvnGraphicsContext;

namespace lvn
{
    LvnResult implVkInitGraphicsContext(LvnGraphicsContext* graphicsContext);
    void      implVkTerminateGraphicsContext();
}

#endif /* !HG_LVN_VULKAN_H */
