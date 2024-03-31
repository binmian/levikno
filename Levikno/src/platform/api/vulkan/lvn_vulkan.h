#ifndef HG_LEVIKNO_VULKAN_H
#define HG_LEVIKNO_VULKAN_H

#include "levikno.h"
#include "levikno_internal.h"

namespace lvn
{
	LvnResult vksImplCreateContext(LvnGraphicsContext* graphicsContext, bool enableValidation);
	void vksImplTerminateContext();
	void vksImplGetPhysicalDevices(LvnPhysicalDevice** pPhysicalDevices, uint32_t* physicalDeviceCount);
	LvnResult vksImplRenderInit(LvnRenderInitInfo* renderBackends);

	LvnResult vksImplCreateShaderFromSrc(LvnShader* shader, LvnShaderCreateInfo* createInfo);
	LvnResult vksImplCreateShaderFromFileSrc(LvnShader* shader, LvnShaderCreateInfo* createInfo);
	LvnResult vksImplCreateShaderFromFileBin(LvnShader* shader, LvnShaderCreateInfo* createInfo);
	LvnResult vksImplCreateDescriptorLayout(LvnDescriptorLayout* descriptorLayout, LvnDescriptorLayoutCreateInfo* createInfo);
	LvnResult vksImplCreatePipeline(LvnPipeline* pipeline, LvnPipelineCreateInfo* createInfo);
	LvnResult vksImplCreateFrameBuffer(LvnFrameBuffer* frameBuffer, LvnFrameBufferCreateInfo* createInfo);
	LvnResult vksImplCreateBuffer(LvnBuffer* vertexArrayBuffer, LvnBufferCreateInfo* createInfo);
	LvnResult vksImplCreateUniformBuffer(LvnUniformBuffer* uniformBuffer, LvnUniformBufferCreateInfo* createInfo);
	LvnResult vksImplCreateTexture(LvnTexture* texture, LvnTextureCreateInfo* createInfo);

	void vksImplDestroyShader(LvnShader* shader);
	void vksImplDestroyDescriptorLayout(LvnDescriptorLayout* descriptorLayout);
	void vksImplDestroyPipeline(LvnPipeline* pipeline);
	void vksImplDestroyFrameBuffer(LvnFrameBuffer* frameBuffer);
	void vksImplDestroyBuffer(LvnBuffer* vertexArrayBuffer);
	void vksImplDestroyUniformBuffer(LvnUniformBuffer* uniformBuffer);
	void vksImplDestroyTexture(LvnTexture* texture);

	void vksImplRenderClearColor(LvnWindow* window, float r, float g, float b, float a);
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
	void vksImplRenderCmdBindDescriptorLayout(LvnWindow* window, LvnPipeline* pipeline, LvnDescriptorLayout* descriptorLayout);
	void vksImplRenderCmdBeginFrameBuffer(LvnWindow* window, LvnFrameBuffer* frameBuffer);
	void vksImplRenderCmdEndFrameBuffer(LvnWindow* window, LvnFrameBuffer* frameBuffer);

	void vksImplSetDefaultPipelineSpecification(LvnPipelineSpecification* pipelineSpecification);
	LvnPipelineSpecification vksImplGetDefaultPipelineSpecification();
	void vksImplUpdateUniformBufferData(LvnWindow* window, LvnUniformBuffer* uniformBuffer, void* data, uint64_t size);
	void vksImplUpdateDescriptorLayoutData(LvnDescriptorLayout* descriptorLayout, LvnDescriptorUpdateInfo* pUpdateInfo, uint32_t count);
	LvnTexture* vksImplGetFrameBufferImage(LvnFrameBuffer* framebuffer, uint32_t attachmentIndex);
	LvnRenderPass* vksImplGetFrameBufferRenderPass(LvnFrameBuffer* frameBuffer);
	void vksImplUpdateFrameBuffer(LvnFrameBuffer* frameBuffer, uint32_t width, uint32_t height);
	void vksImplSetFrameBufferClearColor(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex, float r, float g, float b, float a);
}

#endif
