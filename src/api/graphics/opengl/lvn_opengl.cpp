#include "lvn_opengl.h"
#include "levikno.h"

#include <cstdint>
#include <cstring>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


static const char* s_OpenGLDeviceName = "opengl device";
static uint32_t s_OglVersionMajor = 4;
static uint32_t s_OglVersionMinor = 6;
static LvnPhysicalDevice s_OglPhysicalDevice;

static LvnPipelineSpecification s_DefaultOglPipelineSpecification;

namespace lvn
{

namespace ogls 
{
	static LvnResult checkErrorCode();
	static LvnResult checkShaderError(uint32_t shader, GLenum type, const char* shaderSrc);;
	static uint32_t getVertexAttributeSizeEnum(LvnVertexDataType type);
	static GLenum getVertexAttributeFormatEnum(LvnVertexDataType type);
	static void initDefaultOglPipelineSpecification();

	static LvnResult checkErrorCode()
	{
		GLenum err;
		while((err = glGetError()) != GL_NO_ERROR)
		{
			switch (err)
			{
				case GL_INVALID_ENUM: { LVN_CORE_ERROR("[opengl]: invalid enum value"); break; }
				case GL_INVALID_VALUE: { LVN_CORE_ERROR("[opengl]: invalid parameter value"); break; }
				case GL_INVALID_INDEX: { LVN_CORE_ERROR("[opengl]: invalid operation, state for a command is invalid for its given parameters"); break; }
				case GL_STACK_OVERFLOW: { LVN_CORE_ERROR("[opengl]: stack overflow, stack pushing operation causes stack overflow"); }
				case GL_STACK_UNDERFLOW: { LVN_CORE_ERROR("[opengl]: stack underflow, stach popping operation occurs while stack is at its lowest point"); break; }
				case GL_OUT_OF_MEMORY: { LVN_CORE_ERROR("[opengl]: out of memory, memory allocation cannot allocate enough memory"); break; }
				case GL_INVALID_FRAMEBUFFER_OPERATION: { LVN_CORE_ERROR("[opengl]: reading or writing to a frambuffer is not complete"); break; }
			}
		}

		return err == 0 ? Lvn_Result_Success : Lvn_Result_Failure;
	}

	static LvnResult checkShaderError(uint32_t shader, GLenum type, const char* shaderSrc)
	{
		GLint hasCompiled;
		char infoLog[1024];
		if (type != GL_PROGRAM)
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
			if (hasCompiled == GL_FALSE)
			{
				const char* shaderType;
				if (type == GL_VERTEX_SHADER) { shaderType = "vertex"; }
				else if (type == GL_FRAGMENT_SHADER) { shaderType = "fragment"; }

				glGetShaderInfoLog(shader, 1024, 0, infoLog);
				LVN_CORE_ERROR("[opengl]: [%s] shader compilation error: %s, source: %s", shaderType, infoLog, shaderSrc);

				glDeleteShader(shader);
				return Lvn_Result_Failure;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
			if (hasCompiled == GL_FALSE)
			{
				glGetShaderInfoLog(shader, 1024, 0, infoLog);
				LVN_CORE_ERROR("[opengl]: [program] shader linking error: %s, source: %s", infoLog, shaderSrc);
				glDeleteProgram(shader);
				return Lvn_Result_Failure;
			}
		}

		return Lvn_Result_Success;
	}

	static uint32_t getVertexAttributeSizeEnum(LvnVertexDataType type)
	{
		switch (type)
		{
			case Lvn_VertexDataType_None: { return 0; }
			case Lvn_VertexDataType_Float: { return 1; }
			case Lvn_VertexDataType_Double: { return 1; }
			case Lvn_VertexDataType_Int: { return 1; }
			case Lvn_VertexDataType_UnsignedInt: { return 1; }
			case Lvn_VertexDataType_Bool: { return 1; }
			case Lvn_VertexDataType_Vec2: { return 2; }
			case Lvn_VertexDataType_Vec3: { return 3; }
			case Lvn_VertexDataType_Vec4: { return 4; }
			case Lvn_VertexDataType_Vec2i: { return 2; }
			case Lvn_VertexDataType_Vec3i: { return 3; }
			case Lvn_VertexDataType_Vec4i: { return 4; }
			case Lvn_VertexDataType_Vec2ui: { return 2; }
			case Lvn_VertexDataType_Vec3ui: { return 3; }
			case Lvn_VertexDataType_Vec4ui: { return 4; }
			case Lvn_VertexDataType_Vec2d: { return 2; }
			case Lvn_VertexDataType_Vec3d: { return 3; }
			case Lvn_VertexDataType_Vec4d: { return 4; }
			default:
			{
				LVN_CORE_WARN("uknown vertex attribute format type enum (%d)", type);
				return 0;
			}
		}
	}

	static GLenum getVertexAttributeFormatEnum(LvnVertexDataType type)
	{
		switch (type)
		{
			case Lvn_VertexDataType_None: { return GL_NONE; }
			case Lvn_VertexDataType_Float: { return GL_FLOAT; }
			case Lvn_VertexDataType_Double: { return GL_DOUBLE; }
			case Lvn_VertexDataType_Int: { return GL_INT; }
			case Lvn_VertexDataType_UnsignedInt: { return GL_UNSIGNED_INT; }
			case Lvn_VertexDataType_Bool: { return GL_BOOL; }
			case Lvn_VertexDataType_Vec2: { return GL_FLOAT; }
			case Lvn_VertexDataType_Vec3: { return GL_FLOAT; }
			case Lvn_VertexDataType_Vec4: { return GL_FLOAT; }
			case Lvn_VertexDataType_Vec2i: { return GL_INT; }
			case Lvn_VertexDataType_Vec3i: { return GL_INT; }
			case Lvn_VertexDataType_Vec4i: { return GL_INT; }
			case Lvn_VertexDataType_Vec2ui: { return GL_UNSIGNED_INT; }
			case Lvn_VertexDataType_Vec3ui: { return GL_UNSIGNED_INT; }
			case Lvn_VertexDataType_Vec4ui: { return GL_UNSIGNED_INT; }
			case Lvn_VertexDataType_Vec2d: { return GL_DOUBLE; }
			case Lvn_VertexDataType_Vec3d: { return GL_DOUBLE; }
			case Lvn_VertexDataType_Vec4d: { return GL_DOUBLE; }
			default:
			{
				LVN_CORE_WARN("uknown vertex attribute format type enum (%d)", type);
				return GL_NONE;
			}
		}
	}

	static void initDefaultOglPipelineSpecification()
	{
		// Input Assembly
		s_DefaultOglPipelineSpecification.inputAssembly.topology = Lvn_TopologyType_Triangle;
		s_DefaultOglPipelineSpecification.inputAssembly.primitiveRestartEnable = false;

		// Viewport
		s_DefaultOglPipelineSpecification.viewport.x = 0.0f;
		s_DefaultOglPipelineSpecification.viewport.y = 0.0f;
		s_DefaultOglPipelineSpecification.viewport.width = 0.0f;
		s_DefaultOglPipelineSpecification.viewport.height = 0.0f;
		s_DefaultOglPipelineSpecification.viewport.minDepth = 0.0f;
		s_DefaultOglPipelineSpecification.viewport.maxDepth = 1.0f;

		// Scissor
		s_DefaultOglPipelineSpecification.scissor.extent = { 0, 0 };
		s_DefaultOglPipelineSpecification.scissor.offset = { 0, 0 };

		// Rasterizer
		s_DefaultOglPipelineSpecification.rasterizer.depthClampEnable = false;
		s_DefaultOglPipelineSpecification.rasterizer.rasterizerDiscardEnable = false;
		s_DefaultOglPipelineSpecification.rasterizer.lineWidth = 1.0f;
		s_DefaultOglPipelineSpecification.rasterizer.cullMode = Lvn_CullFaceMode_Disable;
		s_DefaultOglPipelineSpecification.rasterizer.frontFace = Lvn_CullFrontFace_Clockwise;
		s_DefaultOglPipelineSpecification.rasterizer.depthBiasEnable = false;
		s_DefaultOglPipelineSpecification.rasterizer.depthBiasConstantFactor = 0.0f; // Optional
		s_DefaultOglPipelineSpecification.rasterizer.depthBiasClamp = 0.0f; // Optional
		s_DefaultOglPipelineSpecification.rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

		// MultiSampling
		s_DefaultOglPipelineSpecification.multisampling.sampleShadingEnable = false;
		s_DefaultOglPipelineSpecification.multisampling.rasterizationSamples = Lvn_SampleCount_1_Bit;
		s_DefaultOglPipelineSpecification.multisampling.minSampleShading = 1.0f; // Optional
		s_DefaultOglPipelineSpecification.multisampling.sampleMask = nullptr; // Optional
		s_DefaultOglPipelineSpecification.multisampling.alphaToCoverageEnable = false; // Optional
		s_DefaultOglPipelineSpecification.multisampling.alphaToOneEnable = false; // Optional

		// Color Attachments
		s_DefaultOglPipelineSpecification.colorBlend.colorBlendAttachmentCount = 0;
		s_DefaultOglPipelineSpecification.colorBlend.pColorBlendAttachments = nullptr;

		// Color Blend
		s_DefaultOglPipelineSpecification.colorBlend.logicOpEnable = false;
		s_DefaultOglPipelineSpecification.colorBlend.blendConstants[0] = 0.0f; // Optional
		s_DefaultOglPipelineSpecification.colorBlend.blendConstants[1] = 0.0f; // Optional
		s_DefaultOglPipelineSpecification.colorBlend.blendConstants[2] = 0.0f; // Optional
		s_DefaultOglPipelineSpecification.colorBlend.blendConstants[3] = 0.0f; // Optional

		// Depth Stencil
		s_DefaultOglPipelineSpecification.depthstencil.enableDepth = false;
		s_DefaultOglPipelineSpecification.depthstencil.depthOpCompare = Lvn_CompareOperation_Never;
		s_DefaultOglPipelineSpecification.depthstencil.enableStencil = false;
		s_DefaultOglPipelineSpecification.depthstencil.stencil.compareMask = 0x00;
		s_DefaultOglPipelineSpecification.depthstencil.stencil.writeMask = 0x00;
		s_DefaultOglPipelineSpecification.depthstencil.stencil.reference = 0;
		s_DefaultOglPipelineSpecification.depthstencil.stencil.compareOp = Lvn_CompareOperation_Never;
		s_DefaultOglPipelineSpecification.depthstencil.stencil.depthFailOp = Lvn_StencilOperation_Keep;
		s_DefaultOglPipelineSpecification.depthstencil.stencil.failOp = Lvn_StencilOperation_Keep;
		s_DefaultOglPipelineSpecification.depthstencil.stencil.passOp = Lvn_StencilOperation_Keep;
	}

} /* namespace ogls */


LvnResult oglsImplCreateContext(LvnGraphicsContext* graphicsContext)
{
	graphicsContext->getPhysicalDevices = oglsImplGetPhysicalDevices;
	graphicsContext->checkPhysicalDeviceSupport = oglsImplCheckPhysicalDeviceSupport;
	graphicsContext->renderInit = oglsImplRenderInit;
	graphicsContext->createShaderFromSrc = oglsImplCreateShaderFromSrc;
	graphicsContext->createShaderFromFileSrc = oglsImplCreateShaderFromFileSrc;
	graphicsContext->createShaderFromFileBin = oglsImplCreateShaderFromFileBin;
	graphicsContext->createDescriptorLayout = oglsImplCreateDescriptorLayout;
	graphicsContext->createDescriptorSet = oglsImplCreateDescriptorSet;
	graphicsContext->createPipeline = oglsImplCreatePipeline;
	graphicsContext->createFrameBuffer = oglsImplCreateFrameBuffer;
	graphicsContext->createBuffer = oglsImplCreateBuffer;
	graphicsContext->createUniformBuffer = oglsImplCreateUniformBuffer;
	graphicsContext->createTexture = oglsImplCreateTexture;
	graphicsContext->createCubemap = oglsImplCreateCubemap;
	
	graphicsContext->destroyShader = oglsImplDestroyShader;
	graphicsContext->destroyDescriptorLayout = oglsImplDestroyDescriptorLayout;
	graphicsContext->destroyDescriptorSet = oglsImplDestroyDescriptorSet;
	graphicsContext->destroyPipeline = oglsImplDestroyPipeline;
	graphicsContext->destroyFrameBuffer = oglsImplDestroyFrameBuffer;
	graphicsContext->destroyBuffer = oglsImplDestroyBuffer;
	graphicsContext->destroyUniformBuffer = oglsImplDestroyUniformBuffer;
	graphicsContext->destroyTexture = oglsImplDestroyTexture;
	graphicsContext->destroyCubemap = oglsImplDestroyCubemap;

	graphicsContext->renderClearColor = oglsImplRenderClearColor;
	graphicsContext->renderCmdDraw = oglsImplRenderCmdDraw;
	graphicsContext->renderCmdDrawIndexed = oglsImplRenderCmdDrawIndexed;
	graphicsContext->renderCmdDrawInstanced = oglsImplRenderCmdDrawInstanced;
	graphicsContext->renderCmdDrawIndexedInstanced = oglsImplRenderCmdDrawIndexedInstanced;
	graphicsContext->renderCmdSetStencilReference = oglsImplRenderCmdSetStencilReference;
	graphicsContext->renderCmdSetStencilMask = oglsImplRenderCmdSetStencilMask;
	graphicsContext->renderBeginNextFrame = oglsImplRenderBeginNextFrame;
	graphicsContext->renderDrawSubmit = oglsImplRenderDrawSubmit;
	graphicsContext->renderBeginCommandRecording = oglsImplRenderBeginCommandRecording;
	graphicsContext->renderEndCommandRecording = oglsImplRenderEndCommandRecording;
	graphicsContext->renderCmdBeginRenderPass = oglsImplRenderCmdBeginRenderPass;
	graphicsContext->renderCmdEndRenderPass = oglsImplRenderCmdEndRenderPass;
	graphicsContext->renderCmdBindPipeline = oglsImplRenderCmdBindPipeline;
	graphicsContext->renderCmdBindVertexBuffer = oglsImplRenderCmdBindVertexBuffer;
	graphicsContext->renderCmdBindIndexBuffer = oglsImplRenderCmdBindIndexBuffer;
	graphicsContext->renderCmdBindDescriptorSets = oglsImplRenderCmdBindDescriptorSets;
	graphicsContext->renderCmdBeginFrameBuffer = oglsImplRenderCmdBeginFrameBuffer;
	graphicsContext->renderCmdEndFrameBuffer = oglsImplRenderCmdEndFrameBuffer;

	graphicsContext->setDefaultPipelineSpecification = oglsImplSetDefaultPipelineSpecification;
	graphicsContext->getDefaultPipelineSpecification = oglsImplGetDefaultPipelineSpecification;
	graphicsContext->bufferUpdateVertexData = oglsImplBufferUpdateVertexData;
	graphicsContext->bufferUpdateIndexData = oglsImplBufferUpdateIndexData;
	graphicsContext->bufferResizeVertexBuffer = oglsImplBufferResizeVertexBuffer;
	graphicsContext->bufferResizeIndexBuffer = oglsImplBufferResizeIndexBuffer;
	graphicsContext->updateUniformBufferData = oglsImplUpdateUniformBufferData;
	graphicsContext->updateDescriptorSetData = oglsImplUpdateDescriptorSetData;
	graphicsContext->getFrameBufferImage = oglsImplGetFrameBufferImage;
	graphicsContext->getFrameBufferRenderPass = oglsImplGetFrameBufferRenderPass;
	graphicsContext->updateFrameBuffer = oglsImplUpdateFrameBuffer;
	graphicsContext->setFrameBufferClearColor = oglsImplSetFrameBufferClearColor;

	ogls::initDefaultOglPipelineSpecification();

	// NOTE: opengl does not support any enumerated physical devices so we just create a dummy device
	LvnPhysicalDeviceInfo info{};
	memcpy(info.name, s_OpenGLDeviceName, strlen(s_OpenGLDeviceName) + 1);
	info.type = Lvn_PhysicalDeviceType_Other;
	info.apiVersion = (((uint32_t)(s_OglVersionMajor)) << 22) | (((uint32_t)(s_OglVersionMinor)) << 12);
	info.driverVersion = 0;

	LvnPhysicalDevice physicalDevice{};
	physicalDevice.device = nullptr;
	physicalDevice.info = info;

	s_OglPhysicalDevice = physicalDevice;

	return Lvn_Result_Success;
}

void oglsImplTerminateContext()
{
	
}

void oglsImplGetPhysicalDevices(LvnPhysicalDevice** pPhysicalDevices, uint32_t* physicalDeviceCount)
{
	if (pPhysicalDevices == nullptr)
	{
		*physicalDeviceCount = 1;
		return;
	}

	LvnPhysicalDevice* physicalDevice = *pPhysicalDevices;
	physicalDevice = &s_OglPhysicalDevice;
}

LvnResult oglsImplCheckPhysicalDeviceSupport(LvnPhysicalDevice* physicalDevice)
{
	return Lvn_Result_Success;
}

LvnResult oglsImplRenderInit(LvnRenderInitInfo* renderBackends)
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create dummy window to init opengl
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	GLFWwindow* glfwWindow = glfwCreateWindow(1, 1, "", nullptr, nullptr);
	glfwMakeContextCurrent(glfwWindow);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		LVN_CORE_ERROR("[opengl]: failed to initialize glad");
		return Lvn_Result_Failure;
	}

	glfwDestroyWindow(glfwWindow);
	glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

	return Lvn_Result_Success;
}


LvnResult oglsImplCreateShaderFromSrc(LvnShader* shader, LvnShaderCreateInfo* createInfo)
{
	uint32_t vertexShader, fragmentShader;

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &createInfo->vertexSrc, NULL);
	glCompileShader(vertexShader);
	if (ogls::checkShaderError(vertexShader, GL_VERTEX_SHADER, createInfo->vertexSrc) != Lvn_Result_Success)
	{
		LVN_CORE_ERROR("[opengl]: failed to create [vertex] shader module (id:%u) when creating shader (%p)", vertexShader, shader);
		return Lvn_Result_Failure;
	}

	shader->nativeVertexShaderModule = lvn::memAlloc(sizeof(uint32_t));
	memcpy(shader->nativeVertexShaderModule, &vertexShader, sizeof(uint32_t));

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &createInfo->fragmentSrc, NULL);
	glCompileShader(fragmentShader);
	if (ogls::checkShaderError(fragmentShader, GL_FRAGMENT_SHADER, createInfo->fragmentSrc) != Lvn_Result_Success)
	{
		LVN_CORE_ERROR("[opengl]: failed to create [fragment] shader module (id:%u) when creating shader (%p)", fragmentShader, shader);
		return Lvn_Result_Failure;
	}

	shader->nativeFragmentShaderModule = lvn::memAlloc(sizeof(uint32_t));
	memcpy(shader->nativeFragmentShaderModule, &fragmentShader, sizeof(uint32_t));

	return Lvn_Result_Success;
}

LvnResult oglsImplCreateShaderFromFileSrc(LvnShader* shader, LvnShaderCreateInfo* createInfo)
{
	
}

LvnResult oglsImplCreateShaderFromFileBin(LvnShader* shader, LvnShaderCreateInfo* createInfo)
{
	
}

LvnResult oglsImplCreateDescriptorLayout(LvnDescriptorLayout* descriptorLayout, LvnDescriptorLayoutCreateInfo* createInfo)
{
	
}

LvnResult oglsImplCreateDescriptorSet(LvnDescriptorSet* descriptorSet, LvnDescriptorLayout* descriptorLayout)
{
	
}

LvnResult oglsImplCreatePipeline(LvnPipeline* pipeline, LvnPipelineCreateInfo* createInfo)
{
	uint32_t vertexShader = *static_cast<uint32_t*>(createInfo->shader->nativeVertexShaderModule);
	uint32_t fragmentShader = *static_cast<uint32_t*>(createInfo->shader->nativeFragmentShaderModule);

	uint32_t shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	if (ogls::checkShaderError(vertexShader, GL_PROGRAM, "") != Lvn_Result_Success)
	{
		LVN_CORE_ERROR("[opengl]: failed to create shader program (id:%u) when creating pipeline (%p)", shaderProgram, pipeline);
		return Lvn_Result_Failure;
	}

	pipeline->nativePipeline = lvn::memAlloc(sizeof(uint32_t));
	memcpy(pipeline->nativePipeline, &shaderProgram, sizeof(uint32_t));

	return Lvn_Result_Success;
}

LvnResult oglsImplCreateFrameBuffer(LvnFrameBuffer* frameBuffer, LvnFrameBufferCreateInfo* createInfo)
{
	
}

LvnResult oglsImplCreateBuffer(LvnBuffer* buffer, LvnBufferCreateInfo* createInfo)
{
	uint32_t vao, vbo, ibo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);

	glBindVertexArray(vao);

	bool dynamicVertex = false, dynamicIndex = false;

	if (createInfo->type & Lvn_BufferType_DynamicVertex)
		dynamicVertex = true;
	if (createInfo->type & Lvn_BufferType_DynamicIndex)
		dynamicIndex = true;

	// vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, createInfo->vertexBufferSize, createInfo->pVertices, dynamicVertex ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	if (ogls::checkErrorCode() == Lvn_Result_Failure)
	{
		LVN_CORE_ERROR("[opengl]: last error check occurance when creating [vertex] buffer, id: %u, size: %u", vbo, createInfo->vertexBufferSize);
		return Lvn_Result_Failure;
	}

	// index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, createInfo->indexBufferSize, createInfo->pIndices, dynamicIndex ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	if (ogls::checkErrorCode() == Lvn_Result_Failure)
	{
		LVN_CORE_ERROR("[opengl]: last error check occurance when creating [index] buffer, id: %u, size: %u", ibo, createInfo->indexBufferSize);
		return Lvn_Result_Failure;
	}

	// attributes
	for (uint32_t i = 0; i < createInfo->vertexAttributeCount; i++)
	{ 
		glEnableVertexAttribArray(createInfo->pVertexAttributes[i].layout);
		glVertexAttribPointer(
			createInfo->pVertexAttributes[i].layout,
			ogls::getVertexAttributeSizeEnum(createInfo->pVertexAttributes[i].type),
			ogls::getVertexAttributeFormatEnum(createInfo->pVertexAttributes[i].type),
			GL_FALSE,
			createInfo->pVertexBindingDescriptions[0].stride,
			(void*)((uint64_t)createInfo->pVertexAttributes[i].offset));
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// HACK: store the IDs of the vertex and index buffer in the same variable (uint64_t) with each id (uint32_t) in each half of the variable
	memcpy(&buffer->indexOffset, &vbo, sizeof(uint32_t));
	memcpy((char*)(&buffer->indexOffset) + sizeof(uint32_t), &ibo, sizeof(uint32_t));

	// NOTE: the vertex array (vao) is stored in the vertexBufferMemory variable to save space
	buffer->vertexBufferMemory = lvn::memAlloc(sizeof(uint32_t));
	memcpy(buffer->vertexBufferMemory, &vao, sizeof(uint32_t));

	buffer->type = createInfo->type;
	buffer->vertexBuffer = &buffer->indexOffset;
	buffer->vertexBufferSize = createInfo->vertexBufferSize;
	buffer->indexBuffer = (char*)(&buffer->indexOffset) + sizeof(uint32_t);
	buffer->indexBufferSize = createInfo->indexBufferSize;
	buffer->indexBufferMemory = nullptr;

	return Lvn_Result_Success;
}

LvnResult oglsImplCreateUniformBuffer(LvnUniformBuffer* uniformBuffer, LvnUniformBufferCreateInfo* createInfo)
{
	
}

LvnResult oglsImplCreateTexture(LvnTexture* texture, LvnTextureCreateInfo* createInfo)
{
	
}

LvnResult oglsImplCreateCubemap(LvnCubemap* cubemap, LvnCubemapCreateInfo* createInfo)
{
	
}


void oglsImplDestroyShader(LvnShader* shader)
{
	uint32_t* vertexShader = static_cast<uint32_t*>(shader->nativeVertexShaderModule);
	uint32_t* fragmentShader = static_cast<uint32_t*>(shader->nativeFragmentShaderModule);
	
	glDeleteShader(*vertexShader);
	glDeleteShader(*fragmentShader);

	lvn::memFree(vertexShader);
	lvn::memFree(fragmentShader);
}

void oglsImplDestroyDescriptorLayout(LvnDescriptorLayout* descriptorLayout)
{
	
}

void oglsImplDestroyDescriptorSet(LvnDescriptorSet* descriptorSet)
{
	
}

void oglsImplDestroyPipeline(LvnPipeline* pipeline)
{
	uint32_t* shaderProgram = static_cast<uint32_t*>(pipeline->nativePipeline);

	glDeleteProgram(*shaderProgram);

	lvn::memFree(shaderProgram);
}

void oglsImplDestroyFrameBuffer(LvnFrameBuffer* frameBuffer)
{
	
}

void oglsImplDestroyBuffer(LvnBuffer* buffer)
{
	uint32_t* vertexBuffer = static_cast<uint32_t*>(buffer->vertexBuffer);
	uint32_t* indexBuffer = static_cast<uint32_t*>(buffer->indexBuffer);
	uint32_t* vertexArray = static_cast<uint32_t*>(buffer->vertexBufferMemory);

	glDeleteBuffers(1, vertexBuffer);
	glDeleteBuffers(1, indexBuffer);
	glDeleteVertexArrays(1, vertexArray);

	lvn::memFree(vertexArray);
}

void oglsImplDestroyUniformBuffer(LvnUniformBuffer* uniformBuffer)
{
	
}

void oglsImplDestroyTexture(LvnTexture* texture)
{
	
}

void oglsImplDestroyCubemap(LvnCubemap* cubemap)
{
	
}


void oglsImplRenderClearColor(LvnWindow* window, float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT);
}

void oglsImplRenderCmdDraw(LvnWindow* window, uint32_t vertexCount)
{
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}

void oglsImplRenderCmdDrawIndexed(LvnWindow* window, uint32_t indexCount)
{
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}

void oglsImplRenderCmdDrawInstanced(LvnWindow* window, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstInstance)
{
	glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, vertexCount, instanceCount, firstInstance);
}

void oglsImplRenderCmdDrawIndexedInstanced(LvnWindow* window, uint32_t indexCount, uint32_t instanceCount, uint32_t firstInstance)
{
	glDrawElementsInstancedBaseInstance(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0, instanceCount, firstInstance);
}

void oglsImplRenderCmdSetStencilReference(uint32_t reference)
{
	
}

void oglsImplRenderCmdSetStencilMask(uint32_t compareMask, uint32_t writeMask)
{
	
}

void oglsImplRenderBeginNextFrame(LvnWindow* window)
{
	
}

void oglsImplRenderDrawSubmit(LvnWindow* window)
{
	
}

void oglsImplRenderBeginCommandRecording(LvnWindow* window)
{
	
}

void oglsImplRenderEndCommandRecording(LvnWindow* window)
{
	
}

void oglsImplRenderCmdBeginRenderPass(LvnWindow* window)
{
	GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(window->nativeWindow);

	int width, height;
	glfwGetFramebufferSize(glfwWindow, &width, &height);
	glfwMakeContextCurrent(glfwWindow);
	glViewport(0, 0, width, height);
}

void oglsImplRenderCmdEndRenderPass(LvnWindow* window)
{
	
}

void oglsImplRenderCmdBindPipeline(LvnWindow* window, LvnPipeline* pipeline)
{
	uint32_t shaderProgram = *static_cast<uint32_t*>(pipeline->nativePipeline);
	glUseProgram(shaderProgram);
}

void oglsImplRenderCmdBindVertexBuffer(LvnWindow* window, LvnBuffer* buffer)
{
	uint32_t vertexArray = *static_cast<uint32_t*>(buffer->vertexBufferMemory);
	glBindVertexArray(vertexArray);

	// uint32_t vertexBuffer = *static_cast<uint32_t*>(buffer->vertexBuffer);
	// glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
}

void oglsImplRenderCmdBindIndexBuffer(LvnWindow* window, LvnBuffer* buffer)
{
	uint32_t vertexArray = *static_cast<uint32_t*>(buffer->vertexBufferMemory);
	glBindVertexArray(vertexArray);

	// uint32_t indexBuffer = *static_cast<uint32_t*>(buffer->indexBuffer);
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
}

void oglsImplRenderCmdBindDescriptorSets(LvnWindow* window, LvnPipeline* pipeline, uint32_t firstSetIndex, uint32_t descriptorSetCount, LvnDescriptorSet** pDescriptorSet)
{
	
}

void oglsImplRenderCmdBeginFrameBuffer(LvnWindow* window, LvnFrameBuffer* frameBuffer)
{
	
}

void oglsImplRenderCmdEndFrameBuffer(LvnWindow* window, LvnFrameBuffer* frameBuffer)
{
	
}


void oglsImplSetDefaultPipelineSpecification(LvnPipelineSpecification* pipelineSpecification)
{
	s_DefaultOglPipelineSpecification = *pipelineSpecification;
}

LvnPipelineSpecification oglsImplGetDefaultPipelineSpecification()
{
	return s_DefaultOglPipelineSpecification;
}

void oglsImplBufferUpdateVertexData(LvnBuffer* buffer, void* vertices, uint32_t size, uint32_t offset)
{
	
}

void oglsImplBufferUpdateIndexData(LvnBuffer* buffer, uint32_t* indices, uint32_t size, uint32_t offset)
{
	
}

void oglsImplBufferResizeVertexBuffer(LvnBuffer* buffer, uint32_t size)
{
	
}

void oglsImplBufferResizeIndexBuffer(LvnBuffer* buffer, uint32_t size)
{
	
}

void oglsImplUpdateUniformBufferData(LvnWindow* window, LvnUniformBuffer* uniformBuffer, void* data, uint64_t size)
{
	
}

void oglsImplUpdateDescriptorSetData(LvnDescriptorSet* descriptorSet, LvnDescriptorUpdateInfo* pUpdateInfo, uint32_t count)
{
	
}

LvnTexture* oglsImplGetFrameBufferImage(LvnFrameBuffer* framebuffer, uint32_t attachmentIndex)
{
	
}

LvnRenderPass* oglsImplGetFrameBufferRenderPass(LvnFrameBuffer* frameBuffer)
{
	
}

void oglsImplUpdateFrameBuffer(LvnFrameBuffer* frameBuffer, uint32_t width, uint32_t height)
{
	
}

void oglsImplSetFrameBufferClearColor(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex, float r, float g, float b, float a)
{
	
}

} /* namespace lvn */
