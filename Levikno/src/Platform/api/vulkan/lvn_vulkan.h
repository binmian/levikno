#ifndef HG_LEVIKNO_VULKAN_H
#define HG_LEVIKNO_VULKAN_H

#include "Graphics/graphics_internal.h"

namespace lvn
{
	void vksImplCreateContext(LvnGraphicsContext* graphicsContext);
	void vksImplTerminateContext();
	void vksImplGetPhysicalDevices(LvnPhysicalDevice* pPhysicalDevices, uint32_t* deviceCount);
	bool vksImplRenderInit(LvnRendererBackends* renderBackends);

	void vksImplRenderClearColor(const float r, const float g, const float b, const float w);
	void vksImplRenderClear();
	void vksImplRenderDraw(uint32_t vertexCount);
	void vksImplRenderDrawIndexed(uint32_t indexCount);
	void vksImplRenderDrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstInstance);
	void vksImplRenderDrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t firstInstance);
	void vksImplRenderSetStencilReference(uint32_t reference);
	void vksImplRenderSetStencilMask(uint32_t compareMask, uint32_t writeMask);
	void vksImplRenderBeginNextFrame();
	void vksImplRenderDrawSubmit();
	void vksImplRenderBeginRenderPass();
	void vksImplRenderEndRenderPass();
}

#endif