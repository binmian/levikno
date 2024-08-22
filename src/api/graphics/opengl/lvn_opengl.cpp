#include "lvn_opengl.h"
#include "levikno.h"

#include <cstdint>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


namespace lvn
{

static OglBackends* s_OglBackends = nullptr;

namespace ogls 
{
	static LvnResult checkErrorCode();
	static LvnResult checkShaderError(uint32_t shader, GLenum type, const char* shaderSrc);;
	static uint32_t getVertexAttributeSizeEnum(LvnVertexDataType type);
	static GLenum getVertexAttributeFormatEnum(LvnVertexDataType type);
	static GLenum getTextureFilterEnum(LvnTextureFilter filter);
	static GLenum getTextureWrapModeEnum(LvnTextureMode mode);

	static LvnResult checkErrorCode()
	{
		bool errOccurred = false;
		GLenum err;
		while((err = glGetError()) != GL_NO_ERROR)
		{
			switch (err)
			{
				case GL_INVALID_ENUM: { LVN_CORE_ERROR("[opengl]: invalid enum value"); errOccurred = true; break; }
				case GL_INVALID_VALUE: { LVN_CORE_ERROR("[opengl]: invalid parameter value"); errOccurred = true; break; }
				case GL_INVALID_INDEX: { LVN_CORE_ERROR("[opengl]: invalid operation, state for a command is invalid for its given parameters"); errOccurred = true; break; }
				case GL_STACK_OVERFLOW: { LVN_CORE_ERROR("[opengl]: stack overflow, stack pushing operation causes stack overflow"); errOccurred = true; break; }
				case GL_STACK_UNDERFLOW: { LVN_CORE_ERROR("[opengl]: stack underflow, stach popping operation occurs while stack is at its lowest point"); errOccurred = true; break; }
				case GL_OUT_OF_MEMORY: { LVN_CORE_ERROR("[opengl]: out of memory, memory allocation cannot allocate enough memory"); errOccurred = true; break; }
				case GL_INVALID_FRAMEBUFFER_OPERATION: { LVN_CORE_ERROR("[opengl]: reading or writing to a frambuffer is not complete"); errOccurred = true; break; }
			}
		}

		return errOccurred ? Lvn_Result_Failure : Lvn_Result_Success;
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

	static GLenum getTextureFilterEnum(LvnTextureFilter filter)
	{
		switch (filter)
		{
			case Lvn_TextureFilter_Nearest: { return GL_NEAREST; }
			case Lvn_TextureFilter_Linear: { return GL_LINEAR; }

			default:
			{
				LVN_CORE_WARN("unknown sampler filter enum type (%u), setting filter to \'GL_NEAREST\' as default", filter);
				return GL_NEAREST;
			}
		}
	}

	static GLenum getTextureWrapModeEnum(LvnTextureMode mode)
	{
		switch (mode)
		{
			case Lvn_TextureMode_Repeat: { return GL_REPEAT; }
			case Lvn_TextureMode_MirrorRepeat: { return GL_MIRRORED_REPEAT; }
			case Lvn_TextureMode_ClampToEdge: { return GL_CLAMP_TO_EDGE; }
			case Lvn_TextureMode_ClampToBorder: { return GL_CLAMP_TO_BORDER; }

			default:
			{
				LVN_CORE_WARN("unknown sampler address mode enum type (%u), setting mode to \'GL_REPEAT\' as default", mode);
				return GL_REPEAT;
			}
		}
	}
} /* namespace ogls */


LvnResult oglsImplCreateContext(LvnGraphicsContext* graphicsContext)
{
	if (s_OglBackends == nullptr)
	{
		s_OglBackends = new OglBackends();
	}

	s_OglBackends->deviceName = "opengl device";
	s_OglBackends->versionMajor = 4;
	s_OglBackends->versionMinor = 6;
	s_OglBackends->defaultOglPipelineSpecification = lvn::pipelineSpecificationGetConfig();
	s_OglBackends->maxTextureUnitSlots = 32;

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


	// NOTE: opengl does not support any enumerated physical devices so we just create a dummy device
	LvnPhysicalDeviceInfo info{};
	memcpy(info.name, s_OglBackends->deviceName, strlen(s_OglBackends->deviceName) + 1);
	info.type = Lvn_PhysicalDeviceType_Other;
	info.apiVersion = (((uint32_t)(s_OglBackends->versionMajor)) << 22) | (((uint32_t)(s_OglBackends->versionMinor)) << 12);
	info.driverVersion = 0;

	LvnPhysicalDevice physicalDevice{};
	physicalDevice.device = nullptr;
	physicalDevice.info = info;

	s_OglBackends->physicalDevice = physicalDevice;

	return Lvn_Result_Success;
}

void oglsImplTerminateContext()
{
	if (s_OglBackends != nullptr)
	{
		delete s_OglBackends;
	}

	s_OglBackends = nullptr;
}

void oglsImplGetPhysicalDevices(LvnPhysicalDevice** pPhysicalDevices, uint32_t* physicalDeviceCount)
{
	OglBackends* oglBackends = s_OglBackends;

	if (pPhysicalDevices == nullptr)
	{
		*physicalDeviceCount = 1;
		return;
	}

	LvnPhysicalDevice* physicalDevice = *pPhysicalDevices;
	physicalDevice = &oglBackends->physicalDevice;
}

LvnResult oglsImplCheckPhysicalDeviceSupport(LvnPhysicalDevice* physicalDevice)
{
	return Lvn_Result_Success;
}

LvnResult oglsImplRenderInit(LvnRenderInitInfo* renderBackends)
{
	OglBackends* oglBackends = s_OglBackends;

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

	shader->vertexShaderId = vertexShader;

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &createInfo->fragmentSrc, NULL);
	glCompileShader(fragmentShader);
	if (ogls::checkShaderError(fragmentShader, GL_FRAGMENT_SHADER, createInfo->fragmentSrc) != Lvn_Result_Success)
	{
		LVN_CORE_ERROR("[opengl]: failed to create [fragment] shader module (id:%u) when creating shader (%p)", fragmentShader, shader);
		return Lvn_Result_Failure;
	}

	shader->fragmentShaderId = fragmentShader;

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
	OglDescriptorSet descriptorSetLayout{};

	LvnVector<OglDescriptorBinding> uniformDescriptorBindings, textureDescriptorBindings;

	for (uint32_t i = 0; i < createInfo->descriptorBindingCount; i++)
	{
		LvnDescriptorType descriptorType = createInfo->pDescriptorBindings[i].descriptorType;

		if (descriptorType == Lvn_DescriptorType_UniformBuffer || descriptorType == Lvn_DescriptorType_StorageBuffer)
		{
			OglDescriptorBinding descriptorBinding{};
			descriptorBinding.type = descriptorType;
			descriptorBinding.binding = createInfo->pDescriptorBindings[i].binding;
			uniformDescriptorBindings.push_back(descriptorBinding);
		}
		else if (descriptorType == Lvn_DescriptorType_Sampler || descriptorType == Lvn_DescriptorType_SampledImage || descriptorType == Lvn_DescriptorType_CombinedImageSampler)
		{
			OglDescriptorBinding descriptorBinding{};
			descriptorBinding.type = descriptorType;
			descriptorBinding.binding = createInfo->pDescriptorBindings[i].binding;
			textureDescriptorBindings.push_back(descriptorBinding);
		}
	}

	descriptorSetLayout.uniformBufferCount = uniformDescriptorBindings.size();
	descriptorSetLayout.textureCount = textureDescriptorBindings.size();

	descriptorSetLayout.uniformBuffers = (OglDescriptorBinding*)lvn::memAlloc(descriptorSetLayout.uniformBufferCount * sizeof(OglDescriptorBinding));
	descriptorSetLayout.textures = (OglDescriptorBinding*)lvn::memAlloc(descriptorSetLayout.textureCount * sizeof(OglDescriptorBinding));

	memcpy(descriptorSetLayout.uniformBuffers, uniformDescriptorBindings.data(), uniformDescriptorBindings.memsize());
	memcpy(descriptorSetLayout.textures, textureDescriptorBindings.data(), textureDescriptorBindings.memsize());

	descriptorLayout->descriptorPool = nullptr;
	descriptorLayout->descriptorLayout = lvn::memAlloc(sizeof(OglDescriptorSet));
	memcpy(descriptorLayout->descriptorLayout, &descriptorSetLayout, sizeof(OglDescriptorSet));

	return Lvn_Result_Success;
}

LvnResult oglsImplCreateDescriptorSet(LvnDescriptorSet* descriptorSet, LvnDescriptorLayout* descriptorLayout)
{
	descriptorSet->singleSet = lvn::memAlloc(sizeof(OglDescriptorSet));
	memcpy(descriptorSet->singleSet, descriptorLayout->descriptorLayout, sizeof(OglDescriptorSet));

	descriptorSet->descriptorCount = 1;

	return Lvn_Result_Success;
}

LvnResult oglsImplCreatePipeline(LvnPipeline* pipeline, LvnPipelineCreateInfo* createInfo)
{
	uint32_t shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, createInfo->shader->vertexShaderId);
	glAttachShader(shaderProgram, createInfo->shader->fragmentShaderId);
	glLinkProgram(shaderProgram);
	if (ogls::checkShaderError(shaderProgram, GL_PROGRAM, "") != Lvn_Result_Success)
	{
		LVN_CORE_ERROR("[opengl]: failed to create shader program (id:%u) when creating pipeline (%p)", shaderProgram, pipeline);
		return Lvn_Result_Failure;
	}

	pipeline->id = shaderProgram;

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

	buffer->id = vao;
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
	GLenum bufferType = GL_UNIFORM_BUFFER;

	switch (createInfo->type)
	{
		case Lvn_BufferType_Uniform:
		{
			bufferType = GL_UNIFORM_BUFFER;
			break;
		}
		case Lvn_BufferType_Storage:
		{
			bufferType = GL_SHADER_STORAGE_BUFFER;
			break;
		}
		default:
		{
			LVN_CORE_ERROR("unknown buffer type (%u) when creating uniform buffer at (%p)", createInfo->type, uniformBuffer);
			return Lvn_Result_Failure;
		}
	}

	uint32_t id;
	glCreateBuffers(1, &id);
	glNamedBufferData(id, createInfo->size, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(bufferType, createInfo->binding, id);

	uniformBuffer->id = id;
	uniformBuffer->size = createInfo->size;
	
	return Lvn_Result_Success;
}

LvnResult oglsImplCreateTexture(LvnTexture* texture, LvnTextureCreateInfo* createInfo)
{
	uint32_t id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	GLenum texWrapMode = ogls::getTextureWrapModeEnum(createInfo->wrapMode);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texWrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texWrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ogls::getTextureFilterEnum(createInfo->minFilter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ogls::getTextureFilterEnum(createInfo->magFilter));

	GLenum format = createInfo->format == Lvn_TextureFormat_Unorm ? GL_RGB8 : GL_SRGB8;
	GLenum interalFormat = GL_RGB;
	switch (createInfo->imageData.channels)
	{
		case 1: { interalFormat = createInfo->format == Lvn_TextureFormat_Unorm ? GL_R8 : GL_R8; format = GL_RED; break; }
		case 2: { interalFormat = createInfo->format == Lvn_TextureFormat_Unorm ? GL_RG8 : GL_RG8; format = GL_RG; break; }
		case 3: { interalFormat = createInfo->format == Lvn_TextureFormat_Unorm ? GL_RGB8 : GL_SRGB8; format = GL_RGB; break; }
		case 4: { interalFormat = createInfo->format == Lvn_TextureFormat_Unorm ? GL_RGBA8 : GL_SRGB8_ALPHA8; format = GL_RGBA; break; }
	}

	glTexImage2D(GL_TEXTURE_2D, 0, interalFormat, createInfo->imageData.width, createInfo->imageData.height, 0, format, GL_UNSIGNED_BYTE, createInfo->imageData.pixels.data());
	glGenerateMipmap(GL_TEXTURE_2D);
	if (ogls::checkErrorCode() == Lvn_Result_Failure)
	{
		LVN_CORE_ERROR("[opengl]: last error check occurance when creating texture, id: %u, (w:%u,h%u), image data: %p", id, createInfo->imageData.width, createInfo->imageData.height, createInfo->imageData.pixels.data());
		return Lvn_Result_Failure;
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	texture->id = id;

	return Lvn_Result_Success;
}

LvnResult oglsImplCreateCubemap(LvnCubemap* cubemap, LvnCubemapCreateInfo* createInfo)
{
	uint32_t id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	const LvnImageData* texImages[6] = { &createInfo->posx, &createInfo->negx, &createInfo->posy, &createInfo->negy, &createInfo->posz, &createInfo->negz };

	for(uint32_t i = 0; i < 6; i++)
	{
		GLenum texFormat = GL_RGBA;
		switch (texImages[i]->channels)
		{
			case 1: { texFormat = GL_RED; break; }
			case 2: { texFormat = GL_RG; break; }
			case 3: { texFormat = GL_RGB; break; }
			case 4: { texFormat = GL_RGBA; break; }

			default:
			{
				LVN_CORE_ERROR("[opengl]: invalid texture channel format (%u) when creating texture for cubemap (%p)", texImages[i]->channels, cubemap);
				return Lvn_Result_Failure;
			}
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, texFormat, texImages[i]->width, texImages[i]->height, 0, texFormat, GL_UNSIGNED_BYTE, texImages[i]->pixels.data());
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	LvnTexture texData{};
	texData.id = id;

	cubemap->textureData = texData;

	return Lvn_Result_Success;
}


void oglsImplDestroyShader(LvnShader* shader)
{
	glDeleteShader(shader->vertexShaderId);
	glDeleteShader(shader->fragmentShaderId);
}

void oglsImplDestroyDescriptorLayout(LvnDescriptorLayout* descriptorLayout)
{
	OglDescriptorSet* descriptorSetLayout = static_cast<OglDescriptorSet*>(descriptorLayout->descriptorLayout);
	lvn::memFree(descriptorSetLayout->uniformBuffers);
	lvn::memFree(descriptorSetLayout->textures);

	lvn::memFree(descriptorLayout->descriptorLayout);
}

void oglsImplDestroyDescriptorSet(LvnDescriptorSet* descriptorSet)
{
	lvn::memFree(descriptorSet->descriptorSets);
}

void oglsImplDestroyPipeline(LvnPipeline* pipeline)
{
	glDeleteProgram(pipeline->id);
}

void oglsImplDestroyFrameBuffer(LvnFrameBuffer* frameBuffer)
{
	
}

void oglsImplDestroyBuffer(LvnBuffer* buffer)
{
	uint32_t* vertexBuffer = static_cast<uint32_t*>(buffer->vertexBuffer);
	uint32_t* indexBuffer = static_cast<uint32_t*>(buffer->indexBuffer);

	glDeleteBuffers(1, vertexBuffer);
	glDeleteBuffers(1, indexBuffer);
	glDeleteVertexArrays(1, &buffer->id);
}

void oglsImplDestroyUniformBuffer(LvnUniformBuffer* uniformBuffer)
{
	glDeleteBuffers(1, &uniformBuffer->id);
}

void oglsImplDestroyTexture(LvnTexture* texture)
{
	glDeleteTextures(1, &texture->id);
}

void oglsImplDestroyCubemap(LvnCubemap* cubemap)
{
	glDeleteTextures(1, &cubemap->textureData.id);
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
	glUseProgram(pipeline->id);
}

void oglsImplRenderCmdBindVertexBuffer(LvnWindow* window, LvnBuffer* buffer)
{
	glBindVertexArray(buffer->id);

	// uint32_t vertexBuffer = *static_cast<uint32_t*>(buffer->vertexBuffer);
	// glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
}

void oglsImplRenderCmdBindIndexBuffer(LvnWindow* window, LvnBuffer* buffer)
{
	glBindVertexArray(buffer->id);

	// uint32_t indexBuffer = *static_cast<uint32_t*>(buffer->indexBuffer);
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
}

void oglsImplRenderCmdBindDescriptorSets(LvnWindow* window, LvnPipeline* pipeline, uint32_t firstSetIndex, uint32_t descriptorSetCount, LvnDescriptorSet** pDescriptorSet)
{
	OglBackends* oglBackends = s_OglBackends;
	int texCount = 0;

	for (uint32_t i = 0; i < descriptorSetCount; i++)
	{
		OglDescriptorSet* descriptorSetPtr = static_cast<OglDescriptorSet*>(pDescriptorSet[i]->singleSet);

		for (uint32_t j = 0; j < descriptorSetPtr->textureCount; j++)
		{
			LVN_CORE_ASSERT(texCount < oglBackends->maxTextureUnitSlots, "maximum texture unit slots exceeded");

			glActiveTexture(GL_TEXTURE0 + texCount);
			glBindTexture(GL_TEXTURE_2D, descriptorSetPtr->textures[j].id);
			texCount++;
		}
	}
}

void oglsImplRenderCmdBeginFrameBuffer(LvnWindow* window, LvnFrameBuffer* frameBuffer)
{
	
}

void oglsImplRenderCmdEndFrameBuffer(LvnWindow* window, LvnFrameBuffer* frameBuffer)
{
	
}

void oglsImplBufferUpdateVertexData(LvnBuffer* buffer, void* vertices, uint32_t size, uint32_t offset)
{
	uint32_t vbo = *static_cast<uint32_t*>(buffer->vertexBuffer);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void oglsImplBufferUpdateIndexData(LvnBuffer* buffer, uint32_t* indices, uint32_t size, uint32_t offset)
{
	uint32_t ibo = *static_cast<uint32_t*>(buffer->indexBuffer);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void oglsImplBufferResizeVertexBuffer(LvnBuffer* buffer, uint32_t size)
{
	
}

void oglsImplBufferResizeIndexBuffer(LvnBuffer* buffer, uint32_t size)
{
	
}

void oglsImplUpdateUniformBufferData(LvnWindow* window, LvnUniformBuffer* uniformBuffer, void* data, uint64_t size)
{
	glNamedBufferSubData(uniformBuffer->id, 0, size, data);
}

void oglsImplUpdateDescriptorSetData(LvnDescriptorSet* descriptorSet, LvnDescriptorUpdateInfo* pUpdateInfo, uint32_t count)
{
	OglBackends* oglBackends = s_OglBackends;
	int texCount = 0;

	OglDescriptorSet* descriptorSetPtr = static_cast<OglDescriptorSet*>(descriptorSet->singleSet);

	for (uint32_t i = 0; i < count; i++)
	{
		// uniform buffer
		if (pUpdateInfo[i].descriptorType == Lvn_DescriptorType_UniformBuffer || pUpdateInfo[i].descriptorType == Lvn_DescriptorType_StorageBuffer)
		{
			for (uint32_t j = 0; j < descriptorSetPtr->uniformBufferCount; j++)
			{
				if (descriptorSetPtr->uniformBuffers[j].binding == pUpdateInfo[i].binding)
				{
					descriptorSetPtr->uniformBuffers[j].id = pUpdateInfo[i].bufferInfo->id;
					break;
				}
			}
		}

		// texture image
		else if (pUpdateInfo[i].descriptorType == Lvn_DescriptorType_Sampler || pUpdateInfo[i].descriptorType == Lvn_DescriptorType_SampledImage || pUpdateInfo[i].descriptorType == Lvn_DescriptorType_CombinedImageSampler)
		{
			for (uint32_t j = 0; j < descriptorSetPtr->textureCount; j++)
			{
				if (descriptorSetPtr->textures[j].binding == pUpdateInfo[i].binding)
				{
					descriptorSetPtr->textures[j].id = pUpdateInfo[i].textureInfo->id;
					texCount++;
					break;
				}
			}
		}
	}

	if (texCount > oglBackends->maxTextureUnitSlots)
	{
		LVN_CORE_WARN("[opengl]: bound texture unit slots in descriptor set (%p) exceed the maximum texture unit slots that can be bound to a shader, bound texture units: (%d), maximum texture unit slots: (%d)", descriptorSet, texCount, oglBackends->maxTextureUnitSlots);
	}
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
