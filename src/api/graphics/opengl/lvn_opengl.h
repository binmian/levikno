#ifndef HG_LEVIKNO_OPENGL_H
#define HG_LEVIKNO_OPENGL_H

#include "levikno_internal.h"

namespace lvn
{
	LvnResult oglsImplCreateContext(LvnGraphicsContext* graphicsContext);
	void oglsImplTerminateContext();
	void oglsImplGetPhysicalDevices(LvnPhysicalDevice** pPhysicalDevices, uint32_t* physicalDeviceCount);
	LvnResult oglsImplCheckPhysicalDeviceSupport(LvnPhysicalDevice* physicalDevice);
	LvnResult oglsImplRenderInit(LvnRenderInitInfo* renderBackends);

	LvnResult oglsImplCreateShaderFromSrc(LvnShader* shader, LvnShaderCreateInfo* createInfo);
	LvnResult oglsImplCreateShaderFromFileSrc(LvnShader* shader, LvnShaderCreateInfo* createInfo);
	LvnResult oglsImplCreateShaderFromFileBin(LvnShader* shader, LvnShaderCreateInfo* createInfo);
	LvnResult oglsImplCreateDescriptorLayout(LvnDescriptorLayout* descriptorLayout, LvnDescriptorLayoutCreateInfo* createInfo);
	LvnResult oglsImplCreateDescriptorSet(LvnDescriptorSet* descriptorSet, LvnDescriptorLayout* descriptorLayout);
	LvnResult oglsImplCreatePipeline(LvnPipeline* pipeline, LvnPipelineCreateInfo* createInfo);
	LvnResult oglsImplCreateFrameBuffer(LvnFrameBuffer* frameBuffer, LvnFrameBufferCreateInfo* createInfo);
	LvnResult oglsImplCreateBuffer(LvnBuffer* buffer, LvnBufferCreateInfo* createInfo);
	LvnResult oglsImplCreateUniformBuffer(LvnUniformBuffer* uniformBuffer, LvnUniformBufferCreateInfo* createInfo);
	LvnResult oglsImplCreateTexture(LvnTexture* texture, LvnTextureCreateInfo* createInfo);
	LvnResult oglsImplCreateCubemap(LvnCubemap* cubemap, LvnCubemapCreateInfo* createInfo);

	void oglsImplDestroyShader(LvnShader* shader);
	void oglsImplDestroyDescriptorLayout(LvnDescriptorLayout* descriptorLayout);
	void oglsImplDestroyDescriptorSet(LvnDescriptorSet* descriptorSet);
	void oglsImplDestroyPipeline(LvnPipeline* pipeline);
	void oglsImplDestroyFrameBuffer(LvnFrameBuffer* frameBuffer);
	void oglsImplDestroyBuffer(LvnBuffer* buffer);
	void oglsImplDestroyUniformBuffer(LvnUniformBuffer* uniformBuffer);
	void oglsImplDestroyTexture(LvnTexture* texture);
	void oglsImplDestroyCubemap(LvnCubemap* cubemap);

	void oglsImplRenderClearColor(LvnWindow* window, float r, float g, float b, float a);
	void oglsImplRenderCmdDraw(LvnWindow* window, uint32_t vertexCount);
	void oglsImplRenderCmdDrawIndexed(LvnWindow* window, uint32_t indexCount);
	void oglsImplRenderCmdDrawInstanced(LvnWindow* window, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstInstance);
	void oglsImplRenderCmdDrawIndexedInstanced(LvnWindow* window, uint32_t indexCount, uint32_t instanceCount, uint32_t firstInstance);
	void oglsImplRenderCmdSetStencilReference(uint32_t reference);
	void oglsImplRenderCmdSetStencilMask(uint32_t compareMask, uint32_t writeMask);
	void oglsImplRenderBeginNextFrame(LvnWindow* window);
	void oglsImplRenderDrawSubmit(LvnWindow* window);
	void oglsImplRenderBeginCommandRecording(LvnWindow* window);
	void oglsImplRenderEndCommandRecording(LvnWindow* window);
	void oglsImplRenderCmdBeginRenderPass(LvnWindow* window);
	void oglsImplRenderCmdEndRenderPass(LvnWindow* window);
	void oglsImplRenderCmdBindPipeline(LvnWindow* window, LvnPipeline* pipeline);
	void oglsImplRenderCmdBindVertexBuffer(LvnWindow* window, LvnBuffer* buffer);
	void oglsImplRenderCmdBindIndexBuffer(LvnWindow* window, LvnBuffer* buffer);
	void oglsImplRenderCmdBindDescriptorSets(LvnWindow* window, LvnPipeline* pipeline, uint32_t firstSetIndex, uint32_t descriptorSetCount, LvnDescriptorSet** pDescriptorSet);
	void oglsImplRenderCmdBeginFrameBuffer(LvnWindow* window, LvnFrameBuffer* frameBuffer);
	void oglsImplRenderCmdEndFrameBuffer(LvnWindow* window, LvnFrameBuffer* frameBuffer);

	void oglsImplBufferUpdateVertexData(LvnBuffer* buffer, void* vertices, uint32_t size, uint32_t offset);
	void oglsImplBufferUpdateIndexData(LvnBuffer* buffer, uint32_t* indices, uint32_t size, uint32_t offset);
	void oglsImplBufferResizeVertexBuffer(LvnBuffer* buffer, uint32_t size);
	void oglsImplBufferResizeIndexBuffer(LvnBuffer* buffer, uint32_t size);
	void oglsImplUpdateUniformBufferData(LvnWindow* window, LvnUniformBuffer* uniformBuffer, void* data, uint64_t size);
	void oglsImplUpdateDescriptorSetData(LvnDescriptorSet* descriptorSet, LvnDescriptorUpdateInfo* pUpdateInfo, uint32_t count);
	LvnTexture* oglsImplFrameBufferGetImage(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex);
	LvnRenderPass* oglsImplFrameBufferGetRenderPass(LvnFrameBuffer* frameBuffer);
	void oglsImplFrameBufferResize(LvnFrameBuffer* frameBuffer, uint32_t width, uint32_t height);
	void oglsImplFrameBufferSetClearColor(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex, float r, float g, float b, float a);

	LvnDepthImageFormat oglsImplFindSupportedDepthImageFormat(LvnDepthImageFormat* pDepthImageFormats, uint32_t count);


	void setOglWindowContextValues();

	struct OglDescriptorBinding
	{
		LvnDescriptorType type;
		uint32_t binding;
		uint32_t id;
	};

	struct OglDescriptorSet
	{
		OglDescriptorBinding* uniformBuffers;
		OglDescriptorBinding* textures;

		uint32_t uniformBufferCount;
		uint32_t textureCount;
	};

	struct OglPipelineEnums
	{
		bool enableDepth;
	};

	struct OglFramebufferData
	{
		uint32_t id, msaaId;
		uint32_t x, y, width, height;
		LvnTextureFilter textureFilter;
		LvnTextureMode textureMode;
		LvnSampleCount sampleCount;

		LvnVector<LvnFrameBufferColorAttachment> colorAttachmentSpecifications;
		LvnFrameBufferDepthAttachment depthAttachmentSpecification;
		LvnVector<uint32_t> colorAttachments, msaaColorAttachments;
		uint32_t depthAttachment, msaaDepthAttachment;
		bool multisampling, hasDepth;

		LvnVector<LvnTexture> colorAttachmentTextures;
	};

	struct OglBackends
	{
		const char* deviceName;
		uint32_t versionMajor;
		uint32_t versionMinor;
		LvnPhysicalDevice physicalDevice;
		LvnPipelineSpecification defaultOglPipelineSpecification;

		int maxTextureUnitSlots;
		bool framebufferColorFormatSrgb;
	};
}

#endif
