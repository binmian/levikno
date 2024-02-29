#ifndef HG_LEVIKNO_VULKAN_H
#define HG_LEVIKNO_VULKAN_H

#include "levikno_internal.h"

namespace lvn
{
	LvnResult vksImplCreateContext(LvnGraphicsContext* graphicsContext, bool enableValidationLayers = false);
	void vksImplTerminateContext();
	LvnResult vksImplRenderInit(LvnRendererBackends* renderBackends);

	LvnResult vksImplCreateRenderPass(LvnRenderPass* renderPass, LvnRenderPassCreateInfo* createInfo);
	LvnResult vksImplCreateShaderFromSrc(LvnShader* shader, LvnShaderCreateInfo* createInfo);
	LvnResult vksImplCreateShaderFromFileSrc(LvnShader* shader, LvnShaderCreateInfo* createInfo);
	LvnResult vksImplCreateShaderFromFileBin(LvnShader* shader, LvnShaderCreateInfo* createInfo);
	LvnResult vksImplCreateDescriptorLayout(LvnDescriptorLayout* descriptorLayout, LvnDescriptorLayoutCreateInfo* createInfo);
	LvnResult vksImplCreatePipeline(LvnPipeline* pipeline, LvnPipelineCreateInfo* createInfo);
	LvnResult vksImplCreateFrameBuffer(LvnFrameBuffer* frameBuffer, LvnFrameBufferCreateInfo* createInfo);
	LvnResult vksImplCreateBuffer(LvnBuffer* vertexArrayBuffer, LvnBufferCreateInfo* createInfo);

	void vksImplDestroyRenderPass(LvnRenderPass* renderPass);
	void vksImplDestroyShader(LvnShader* shader);
	void vksImplDestroyDescriptorLayout(LvnDescriptorLayout* descriptorLayout);
	void vksImplDestroyPipeline(LvnPipeline* pipeline);
	void vksImplDestroyFrameBuffer(LvnFrameBuffer* frameBuffer);
	void vksImplDestroyBuffer(LvnBuffer* vertexArrayBuffer);

	void vksImplRenderCmdDraw(LvnWindow* window, uint32_t vertexCount);
	void vksImplRenderCmdDrawIndexed(LvnWindow* window, uint32_t indexCount);
	void vksImplRenderCmdDrawInstanced(LvnWindow* window, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstInstance);
	void vksImplRenderCmdDrawIndexedInstanced(LvnWindow* window, uint32_t indexCount, uint32_t instanceCount, uint32_t firstInstance);
	void vksImplRenderCmdSetStencilReference(uint32_t reference);
	void vksImplRenderCmdSetStencilMask(uint32_t compareMask, uint32_t writeMask);
	void vksImplRenderBeginNextFrame(LvnWindow* window);
	void vksImplRenderDrawSubmit(LvnWindow* window);
	void vksImplRenderBeginCommandRecording(LvnWindow* window);
	void vksImplRenderEndCommandRecording(LvnWindow* window);
	void vksImplRenderCmdBeginRenderPass(LvnWindow* window);
	void vksImplRenderCmdEndRenderPass(LvnWindow* window);
	void vksImplRenderCmdBindPipeline(LvnWindow* window, LvnPipeline* pipeline);
	void vksImplRenderCmdBindVertexBuffer(LvnWindow* window, LvnBuffer* buffer);
	void vksImplRenderCmdBindIndexBuffer(LvnWindow* window, LvnBuffer* buffer);

	void vksImplSetDefaultPipelineSpecification(LvnPipelineSpecification* pipelineSpecification);
	LvnPipelineSpecification vksImplGetDefaultPipelineSpecification();

}

#endif
