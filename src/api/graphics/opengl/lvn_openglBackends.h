#ifndef HG_LEVIKNO_OPENGL_BACKENDS_H
#define HG_LEVIKNO_OPENGL_BACKENDS_H

#include "levikno_internal.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

struct OglDescriptorBinding
{
	LvnDescriptorType type;
	uint32_t binding;
	uint32_t count;
	uint32_t id;
	uint64_t range;
	uint64_t offset;
};

struct OglBindlessTextureBinding
{
	uint32_t ssbo;
	uint32_t binding;
	std::vector<uint64_t> textureHandles;
};

struct OglDescriptorSet
{
	std::vector<OglDescriptorBinding> uniformBuffers;
	std::vector<OglDescriptorBinding> textures;
	std::vector<OglBindlessTextureBinding> bindlessTextures;
};

struct OglPipelineEnums
{
	uint32_t depthCompareOp;
	uint32_t topologyType;
	uint32_t srcBlendFactor;
	uint32_t dstBlendFactor;
	uint32_t cullMode;
	uint32_t frontFace;
	bool enableDepth;
	bool enableBlending;
	bool enableCulling;
};

struct OglFramebufferData
{
	uint32_t id, msaaId;
	uint32_t x, y, width, height;
	LvnTextureFilter textureFilter;
	LvnTextureMode textureMode;
	LvnSampleCount sampleCount;

	std::vector<LvnFrameBufferColorAttachment> colorAttachmentSpecifications;
	LvnFrameBufferDepthAttachment depthAttachmentSpecification;
	std::vector<uint32_t> colorAttachments, msaaColorAttachments;
	uint32_t depthAttachment, msaaDepthAttachment;
	bool multisampling, hasDepth;

	std::vector<LvnTexture> colorAttachmentTextures;
};

struct OglSampler
{
	LvnTextureFilter minFilter, magFilter;
	LvnTextureMode wrapS, wrapT, wrapR;
};

struct OglBackends
{
	GLFWwindow* windowContext;
	const char* deviceName;
	uint32_t versionMajor;
	uint32_t versionMinor;
	LvnPhysicalDevice physicalDevice;
	LvnPipelineSpecification defaultOglPipelineSpecification;

	int maxTextureUnitSlots;
	bool framebufferColorFormatSrgb;
};


namespace lvn
{
	void oglsImplRecordCmdDraw(LvnWindow* window, uint32_t vertexCount);
	void oglsImplRecordCmdDrawIndexed(LvnWindow* window, uint32_t indexCount);
	void oglsImplRecordCmdDrawInstanced(LvnWindow* window, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstInstance);
	void oglsImplRecordCmdDrawIndexedInstanced(LvnWindow* window, uint32_t indexCount, uint32_t instanceCount, uint32_t firstInstance);
	void oglsImplRecordCmdSetStencilReference(uint32_t reference);
	void oglsImplRecordCmdSetStencilMask(uint32_t compareMask, uint32_t writeMask);
	void oglsImplRecordCmdBeginRenderPass(LvnWindow* window, float r, float g, float b, float a);
	void oglsImplRecordCmdEndRenderPass(LvnWindow* window);
	void oglsImplRecordCmdBindPipeline(LvnWindow* window, LvnPipeline* pipeline);
	void oglsImplRecordCmdBindVertexBuffer(LvnWindow* window, uint32_t firstBinding, uint32_t bindingCount, LvnBuffer** pBuffers, uint64_t* pOffsets);
	void oglsImplRecordCmdBindIndexBuffer(LvnWindow* window, LvnBuffer* buffer, uint64_t offset);
	void oglsImplRecordCmdBindDescriptorSets(LvnWindow* window, LvnPipeline* pipeline, uint32_t firstSetIndex, uint32_t descriptorSetCount, LvnDescriptorSet** pDescriptorSets);
	void oglsImplRecordCmdBeginFrameBuffer(LvnWindow* window, LvnFrameBuffer* frameBuffer);
	void oglsImplRecordCmdEndFrameBuffer(LvnWindow* window, LvnFrameBuffer* frameBuffer);

	void oglsImplDrawBuffCmdDraw(void* data);
	void oglsImplDrawBuffCmdDrawIndexed(void* data);
	void oglsImplDrawBuffCmdDrawInstanced(void* data);
	void oglsImplDrawBuffCmdDrawIndexedInstanced(void* data);
	void oglsImplDrawBuffCmdSetStencilReference(void* data);
	void oglsImplDrawBuffCmdSetStencilMask(void* data);
	void oglsImplDrawBuffCmdBeginRenderPass(void* data);
	void oglsImplDrawBuffCmdEndRenderPass(void* data);
	void oglsImplDrawBuffCmdBindPipeline(void* data);
	void oglsImplDrawBuffCmdBindVertexBuffer(void* data);
	void oglsImplDrawBuffCmdBindIndexBuffer(void* data);
	void oglsImplDrawBuffCmdBindDescriptorSets(void* data);
	void oglsImplDrawBuffCmdBeginFrameBuffer(void* data);
	void oglsImplDrawBuffCmdEndFrameBuffer(void* data);
}


#endif
