#ifndef HG_LEVIKNO_VULKAN_H
#define HG_LEVIKNO_VULKAN_H

#include "levikno_internal.h"

namespace lvn
{
	LvnResult vksImplCreateContext(LvnGraphicsContext* graphicsContext);
	void vksImplTerminateContext();
	void vksImplGetPhysicalDevices(LvnPhysicalDevice** pPhysicalDevices, uint32_t* physicalDeviceCount);
	LvnResult vksImplCheckPhysicalDeviceSupport(LvnPhysicalDevice* physicalDevice);
	LvnResult vksImplRenderInit(LvnRenderInitInfo* renderBackends);

	LvnResult vksImplCreateShaderFromSrc(LvnShader* shader, LvnShaderCreateInfo* createInfo);
	LvnResult vksImplCreateShaderFromFileSrc(LvnShader* shader, LvnShaderCreateInfo* createInfo);
	LvnResult vksImplCreateShaderFromFileBin(LvnShader* shader, LvnShaderCreateInfo* createInfo);
	LvnResult vksImplCreateDescriptorLayout(LvnDescriptorLayout* descriptorLayout, LvnDescriptorLayoutCreateInfo* createInfo);
	LvnResult vksImplCreateDescriptorSet(LvnDescriptorSet* descriptorSet, LvnDescriptorLayout* descriptorLayout);
	LvnResult vksImplCreatePipeline(LvnPipeline* pipeline, LvnPipelineCreateInfo* createInfo);
	LvnResult vksImplCreateFrameBuffer(LvnFrameBuffer* frameBuffer, LvnFrameBufferCreateInfo* createInfo);
	LvnResult vksImplCreateBuffer(LvnBuffer* buffer, LvnBufferCreateInfo* createInfo);
	LvnResult vksImplCreateUniformBuffer(LvnUniformBuffer* uniformBuffer, LvnUniformBufferCreateInfo* createInfo);
	LvnResult vksImplCreateTexture(LvnTexture* texture, LvnTextureCreateInfo* createInfo);
	LvnResult vksImplCreateCubemap(LvnCubemap* cubemap, LvnCubemapCreateInfo* createInfo);

	void vksImplDestroyShader(LvnShader* shader);
	void vksImplDestroyDescriptorLayout(LvnDescriptorLayout* descriptorLayout);
	void vksImplDestroyDescriptorSet(LvnDescriptorSet* descriptorSet);
	void vksImplDestroyPipeline(LvnPipeline* pipeline);
	void vksImplDestroyFrameBuffer(LvnFrameBuffer* frameBuffer);
	void vksImplDestroyBuffer(LvnBuffer* vertexArrayBuffer);
	void vksImplDestroyUniformBuffer(LvnUniformBuffer* uniformBuffer);
	void vksImplDestroyTexture(LvnTexture* texture);
	void vksImplDestroyCubemap(LvnCubemap* cubemap);

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
	void vksImplRenderCmdBindDescriptorSets(LvnWindow* window, LvnPipeline* pipeline, uint32_t firstSetIndex, uint32_t descriptorSetCount, LvnDescriptorSet** pDescriptorSet);
	void vksImplRenderCmdBeginFrameBuffer(LvnWindow* window, LvnFrameBuffer* frameBuffer);
	void vksImplRenderCmdEndFrameBuffer(LvnWindow* window, LvnFrameBuffer* frameBuffer);

	void vksImplBufferUpdateVertexData(LvnBuffer* buffer, void* vertices, uint32_t size, uint32_t offset);
	void vksImplBufferUpdateIndexData(LvnBuffer* buffer, uint32_t* indices, uint32_t size, uint32_t offset);
	void vksImplBufferResizeVertexBuffer(LvnBuffer* buffer, uint32_t size);
	void vksImplBufferResizeIndexBuffer(LvnBuffer* buffer, uint32_t size);
	void vksImplUpdateUniformBufferData(LvnWindow* window, LvnUniformBuffer* uniformBuffer, void* data, uint64_t size);
	void vksImplUpdateDescriptorSetData(LvnDescriptorSet* descriptorSet, LvnDescriptorUpdateInfo* pUpdateInfo, uint32_t count);
	LvnTexture* vksImplGetFrameBufferImage(LvnFrameBuffer* framebuffer, uint32_t attachmentIndex);
	LvnRenderPass* vksImplGetFrameBufferRenderPass(LvnFrameBuffer* frameBuffer);
	void vksImplUpdateFrameBuffer(LvnFrameBuffer* frameBuffer, uint32_t width, uint32_t height);
	void vksImplSetFrameBufferClearColor(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex, float r, float g, float b, float a);
}

#endif
