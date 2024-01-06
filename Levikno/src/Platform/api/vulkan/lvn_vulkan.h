#ifndef HG_LEVIKNO_VULKAN_H
#define HG_LEVIKNO_VULKAN_H

#include "Graphics/graphics_internal.h"

namespace lvn
{
	void vksImplCreateContext(GraphicsContext* graphicsContext, RendererBackends* renderBackends);
	void vksImplTerminateContext();
	void vksImplGetPhysicalDevices(PhysicalDevice* pPhysicalDevices, uint32_t* deviceCount);
}

#endif