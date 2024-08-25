#include "lvn_opengl.h"

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
	static uint32_t getSampleCountEnum(LvnSampleCount samples);
	static GLenum getColorFormat(LvnColorImageFormat texFormat);
	static GLenum getDataFormat(LvnColorImageFormat texFormat);
	static LvnResult updateFrameBuffer(OglFramebufferData* frameBufferData);

	static LvnResult checkErrorCode()
	{
		bool errOccurred = false;
		GLenum err;
		while((err = glGetError()) != GL_NO_ERROR)
		{
			switch (err)
			{
				case GL_INVALID_ENUM: { LVN_CORE_ERROR("[opengl] invalid enum value"); errOccurred = true; break; }
				case GL_INVALID_VALUE: { LVN_CORE_ERROR("[opengl] invalid parameter value"); errOccurred = true; break; }
				case GL_INVALID_INDEX: { LVN_CORE_ERROR("[opengl] invalid operation, state for a command is invalid for its given parameters"); errOccurred = true; break; }
				case GL_STACK_OVERFLOW: { LVN_CORE_ERROR("[opengl] stack overflow, stack pushing operation causes stack overflow"); errOccurred = true; break; }
				case GL_STACK_UNDERFLOW: { LVN_CORE_ERROR("[opengl] stack underflow, stach popping operation occurs while stack is at its lowest point"); errOccurred = true; break; }
				case GL_OUT_OF_MEMORY: { LVN_CORE_ERROR("[opengl] out of memory, memory allocation cannot allocate enough memory"); errOccurred = true; break; }
				case GL_INVALID_FRAMEBUFFER_OPERATION: { LVN_CORE_ERROR("[opengl] reading or writing to a frambuffer is not complete"); errOccurred = true; break; }
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
				LVN_CORE_ERROR("[opengl] [%s] shader compilation error: %s, source: %s", shaderType, infoLog, shaderSrc);

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
				LVN_CORE_ERROR("[opengl] [program] shader linking error: %s, source: %s", infoLog, shaderSrc);
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

	static uint32_t getSampleCountEnum(LvnSampleCount samples)
	{
		switch (samples)
		{
			case Lvn_SampleCount_1_Bit: { return 1; }
			case Lvn_SampleCount_2_Bit: { return 2; }
			case Lvn_SampleCount_4_Bit: { return 4; }
			case Lvn_SampleCount_8_Bit: { return 8; }
			case Lvn_SampleCount_16_Bit: { return 16; }
			case Lvn_SampleCount_32_Bit: { return 32; }
			case Lvn_SampleCount_64_Bit: { return 64; }

			default:
			{
				LVN_CORE_WARN("[opengl] invalid sample value (%u), setting sample value to 1", samples);
				return 1;
			}
		}
	}

	static GLenum getColorFormat(LvnColorImageFormat texFormat)
	{
		switch (texFormat)
		{
			case Lvn_ImageFormat_None: { return GL_NONE; }
			case Lvn_ImageFormat_RedInt: { return GL_R32I; }
			case Lvn_ImageFormat_RGB: { return GL_RGB; }
			case Lvn_ImageFormat_RGBA: { return GL_RGBA; }
			case Lvn_ImageFormat_RGBA8: { return GL_RGBA8; }
			case Lvn_ImageFormat_RGBA16F: { return GL_RGBA16F; }
			case Lvn_ImageFormat_RGBA32F: { return GL_RGBA32F; }

			default:
			{
				LVN_CORE_WARN("[opengl] invalid color image format (%u), setting image format to GL_RGBA8", texFormat);
				return GL_RGBA8;
			}
		}
	}

	static GLenum getDataFormat(LvnColorImageFormat texFormat)
	{
		switch (texFormat)
		{
			case Lvn_ImageFormat_None: { return GL_NONE; }
			case Lvn_ImageFormat_RedInt: { return GL_RED; }
			case Lvn_ImageFormat_RGB: { return GL_RGB; }
			case Lvn_ImageFormat_RGBA: { return GL_RGBA; }
			case Lvn_ImageFormat_RGBA8: { return GL_RGBA; }
			case Lvn_ImageFormat_RGBA16F: { return GL_RGBA; }
			case Lvn_ImageFormat_RGBA32F: { return GL_RGBA; }

			default:
			{
				LVN_CORE_WARN("invalid color data image format, setting data image format to GL_RGBA");
				return GL_RGBA;
			}
		}
	}

	static LvnResult updateFrameBuffer(OglFramebufferData* frameBufferData)
	{
		GLenum texType = frameBufferData->multisampling ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		GLenum texFilter = ogls::getTextureFilterEnum(frameBufferData->textureFilter);
		GLenum texWrapMode = ogls::getTextureWrapModeEnum(frameBufferData->textureMode);

		glGenFramebuffers(1, &frameBufferData->id);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferData->id);

		if (frameBufferData->colorAttachmentSpecifications.size() > 0)
		{
			frameBufferData->colorAttachments.resize(frameBufferData->colorAttachmentSpecifications.size());
			glCreateTextures(texType, frameBufferData->colorAttachments.size(), frameBufferData->colorAttachments.data());

			for (uint32_t i = 0; i < frameBufferData->colorAttachments.size(); i++)
			{
				glBindTexture(texType, frameBufferData->colorAttachments[i]);

				GLenum colorFormat = ogls::getColorFormat(frameBufferData->colorAttachmentSpecifications[i].format);
				GLenum dataFormat = ogls::getDataFormat(frameBufferData->colorAttachmentSpecifications[i].format);

				if (frameBufferData->multisampling)
				{
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, ogls::getSampleCountEnum(frameBufferData->sampleCount), colorFormat, frameBufferData->width, frameBufferData->height, GL_TRUE);
				}
				else
				{
					glTexImage2D(GL_TEXTURE_2D, 0, colorFormat, frameBufferData->width, frameBufferData->height, 0, dataFormat, GL_UNSIGNED_BYTE, nullptr);

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texFilter);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texFilter);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, texWrapMode);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texWrapMode);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texWrapMode);
				}

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, texType, frameBufferData->colorAttachments[i], 0);

				if (ogls::checkErrorCode() == Lvn_Result_Failure)
				{
					LVN_CORE_ERROR("[opengl] last error check occurance when creating framebuffer color texture attachment, id: %u", frameBufferData->colorAttachments[i]);
					return Lvn_Result_Failure;
				}
			}
		}

		if (frameBufferData->hasDepth)
		{
			glCreateTextures(texType, 1, &frameBufferData->depthAttachment);
			glBindTexture(texType, frameBufferData->depthAttachment);

			GLenum format = GL_DEPTH_COMPONENT, attachmentType = GL_DEPTH_ATTACHMENT;
			
			switch (frameBufferData->depthAttachmentSpecification.format)
			{
				case Lvn_ImageFormat_DepthComponent:
				{
					format = GL_DEPTH_COMPONENT;
					attachmentType = GL_DEPTH_ATTACHMENT;
					break;
				}
				case Lvn_ImageFormat_Depth24Stencil8:
				{
					format = GL_DEPTH24_STENCIL8;
					attachmentType = GL_DEPTH_STENCIL_ATTACHMENT;
					break;
				}
				case Lvn_ImageFormat_Depth32Stencil8:
				{
					format = GL_DEPTH24_STENCIL8;
					attachmentType = GL_DEPTH_STENCIL_ATTACHMENT;
					break;
				}
			}

			if (frameBufferData->multisampling)
			{
				glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, ogls::getSampleCountEnum(frameBufferData->sampleCount), format, frameBufferData->width, frameBufferData->height, GL_TRUE);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, format, frameBufferData->width, frameBufferData->height);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texFilter);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texFilter);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, texWrapMode);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texWrapMode);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texWrapMode);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, texType, frameBufferData->depthAttachment, 0);

			if (ogls::checkErrorCode() == Lvn_Result_Failure)
			{
				LVN_CORE_ERROR("[opengl] last error check occurance when creating framebuffer depth texture attachment, id: %u", frameBufferData->depthAttachment);
				return Lvn_Result_Failure;
			}
		}

		if (!frameBufferData->multisampling)
		{
			if (frameBufferData->colorAttachments.size() > 1)
			{
				if (frameBufferData->colorAttachments.size() > 4)
				{
					LVN_CORE_ERROR("[opengl] amount of color attachments is more than four (%u), maximum color attachments must be four or less when creating framebuffer", frameBufferData->colorAttachments.size());
					return Lvn_Result_Failure;
				}

				GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
				glDrawBuffers(static_cast<GLsizei>(frameBufferData->colorAttachments.size()), buffers);
			}
			else if (frameBufferData->colorAttachments.size() == 0) // Only depth-pass
			{
				glDrawBuffer(GL_NONE);
			}
		}

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if(status != GL_FRAMEBUFFER_COMPLETE)
		{
			LVN_CORE_ERROR("[opengl] failed to create framebuffer, framebuffer is not complete, error: %u", status);
			return Lvn_Result_Failure;
		}

		if (ogls::checkErrorCode() == Lvn_Result_Failure)
		{
			LVN_CORE_ERROR("[opengl] last error check occurance when creating framebuffer, id: %u", frameBufferData->id);
			return Lvn_Result_Failure;
		}

		if (frameBufferData->multisampling)
		{
			glGenFramebuffers(1, &frameBufferData->msaaId);
			glBindFramebuffer(GL_FRAMEBUFFER, frameBufferData->msaaId);

			frameBufferData->msaaColorAttachments.resize(frameBufferData->colorAttachments.size());

			glCreateTextures(GL_TEXTURE_2D, frameBufferData->colorAttachments.size(), frameBufferData->msaaColorAttachments.data());

			for (uint32_t i = 0; i < frameBufferData->colorAttachments.size(); i++)
			{
				glBindTexture(GL_TEXTURE_2D, frameBufferData->msaaColorAttachments[i]);

				GLenum colorFormat = ogls::getColorFormat(frameBufferData->colorAttachmentSpecifications[i].format);
				GLenum dataFormat = ogls::getDataFormat(frameBufferData->colorAttachmentSpecifications[i].format);

				glTexImage2D(GL_TEXTURE_2D, 0, colorFormat, frameBufferData->width, frameBufferData->height, 0, dataFormat, GL_UNSIGNED_BYTE, nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texFilter);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texFilter);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, texWrapMode);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texWrapMode);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texWrapMode);

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, frameBufferData->msaaColorAttachments[i], 0);

				if (ogls::checkErrorCode() == Lvn_Result_Failure)
				{
					LVN_CORE_ERROR("[opengl] last error check occurance when creating framebuffer msaa color texture attachment, id: %u", frameBufferData->msaaColorAttachments[i]);
					return Lvn_Result_Failure;
				}
			}

			if (frameBufferData->hasDepth)
			{
				glCreateTextures(GL_TEXTURE_2D, 1, &frameBufferData->msaaDepthAttachment);
				glBindTexture(GL_TEXTURE_2D, frameBufferData->msaaDepthAttachment);

				GLenum format = GL_DEPTH_COMPONENT, attachmentType = GL_DEPTH_ATTACHMENT;

				switch (frameBufferData->depthAttachmentSpecification.format)
				{
					case Lvn_ImageFormat_DepthComponent:
					{
						format = GL_DEPTH_COMPONENT;
						attachmentType = GL_DEPTH_ATTACHMENT;
						break;
					}
					case Lvn_ImageFormat_Depth24Stencil8:
					{
						format = GL_DEPTH24_STENCIL8;
						attachmentType = GL_DEPTH_STENCIL_ATTACHMENT;
						break;
					}
					case Lvn_ImageFormat_Depth32Stencil8:
					{
						format = GL_DEPTH24_STENCIL8;
						attachmentType = GL_DEPTH_STENCIL_ATTACHMENT;
						break;
					}
				}

				glTexStorage2D(GL_TEXTURE_2D, 1, format, frameBufferData->width, frameBufferData->height);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texFilter);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texFilter);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, texWrapMode);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texWrapMode);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texWrapMode);

				glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, texType, frameBufferData->msaaDepthAttachment, 0);

				if (ogls::checkErrorCode() == Lvn_Result_Failure)
				{
					LVN_CORE_ERROR("[opengl] last error check occurance when creating framebuffer msaa depth texture attachment, id: %u", frameBufferData->msaaDepthAttachment);
					return Lvn_Result_Failure;
				}
			}

			if (frameBufferData->colorAttachments.size() > 1)
			{
				if (frameBufferData->colorAttachments.size() > 4)
				{
					LVN_CORE_ERROR("[opengl] amount of color attachments is more than four (%u), maximum color attachments must be four or less when creating framebuffer", frameBufferData->colorAttachments.size());
					return Lvn_Result_Failure;
				}

				GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
				glDrawBuffers(static_cast<GLsizei>(frameBufferData->colorAttachments.size()), buffers);
			}
			else if (frameBufferData->colorAttachments.size() == 0) // Only depth-pass
			{
				glDrawBuffer(GL_NONE);
			}

			// Error checking framebuffer
			status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if (status != GL_FRAMEBUFFER_COMPLETE)
			{
				LVN_CORE_ERROR("[opengl] failed to create framebuffer, framebuffer is not complete, error: %u", status);
				return Lvn_Result_Failure;
			}

			if (ogls::checkErrorCode() == Lvn_Result_Failure)
			{
				LVN_CORE_ERROR("[opengl] last error check occurance when creating msaa framebuffer, id: %u", frameBufferData->msaaId);
				return Lvn_Result_Failure;
			}

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		frameBufferData->colorAttachmentTextures.resize(frameBufferData->colorAttachments.size());

		for (uint32_t i = 0; i < frameBufferData->colorAttachmentTextures.size(); i++)
		{
			LvnTexture colorTexture{};
			colorTexture.id = frameBufferData->multisampling ? frameBufferData->msaaColorAttachments[i] : frameBufferData->colorAttachments[i];
			frameBufferData->colorAttachmentTextures[i] = colorTexture;
		}

		return Lvn_Result_Success;
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
	s_OglBackends->framebufferColorFormatSrgb = graphicsContext->frameBufferColorFormat == Lvn_TextureFormat_Srgb ? true : false;

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
	graphicsContext->frameBufferGetImage = oglsImplFrameBufferGetImage;
	graphicsContext->frameBufferGetRenderPass = oglsImplFrameBufferGetRenderPass;
	graphicsContext->framebufferResize = oglsImplFrameBufferResize;
	graphicsContext->frameBufferSetClearColor = oglsImplFrameBufferSetClearColor;


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
		LVN_CORE_ERROR("[opengl] failed to initialize glad");
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
		LVN_CORE_ERROR("[opengl] failed to create vertex shader module (id:%u) when creating shader (%p)", vertexShader, shader);
		return Lvn_Result_Failure;
	}

	shader->vertexShaderId = vertexShader;

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &createInfo->fragmentSrc, NULL);
	glCompileShader(fragmentShader);
	if (ogls::checkShaderError(fragmentShader, GL_FRAGMENT_SHADER, createInfo->fragmentSrc) != Lvn_Result_Success)
	{
		LVN_CORE_ERROR("[opengl] failed to create fragment shader module (id:%u) when creating shader (%p)", fragmentShader, shader);
		return Lvn_Result_Failure;
	}

	shader->fragmentShaderId = fragmentShader;

	return Lvn_Result_Success;
}

LvnResult oglsImplCreateShaderFromFileSrc(LvnShader* shader, LvnShaderCreateInfo* createInfo)
{
	std::string fileVertSrc = lvn::loadFileSrc(createInfo->vertexSrc);
	std::string fileFragSrc = lvn::loadFileSrc(createInfo->fragmentSrc);

	const char* vertSrc = fileVertSrc.c_str();
	const char* fragSrc = fileFragSrc.c_str();

	uint32_t vertexShader, fragmentShader;

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertSrc, NULL);
	glCompileShader(vertexShader);
	if (ogls::checkShaderError(vertexShader, GL_VERTEX_SHADER, vertSrc) != Lvn_Result_Success)
	{
		LVN_CORE_ERROR("[opengl] failed to create vertex shader module (id:%u) when creating shader (%p)", vertexShader, shader);
		return Lvn_Result_Failure;
	}

	shader->vertexShaderId = vertexShader;

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragSrc, NULL);
	glCompileShader(fragmentShader);
	if (ogls::checkShaderError(fragmentShader, GL_FRAGMENT_SHADER, fragSrc) != Lvn_Result_Success)
	{
		LVN_CORE_ERROR("[opengl] failed to create fragment shader module (id:%u) when creating shader (%p)", fragmentShader, shader);
		return Lvn_Result_Failure;
	}

	shader->fragmentShaderId = fragmentShader;

	return Lvn_Result_Success;
}

LvnResult oglsImplCreateShaderFromFileBin(LvnShader* shader, LvnShaderCreateInfo* createInfo)
{
	LvnData<uint8_t> vertbin = lvn::loadFileSrcBin(createInfo->vertexSrc);
	LvnData<uint8_t> fragbin = lvn::loadFileSrcBin(createInfo->fragmentSrc);

	uint32_t vertexShader, fragmentShader;

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderBinary(1, &vertexShader, GL_SHADER_BINARY_FORMAT_SPIR_V, vertbin.data(), vertbin.size());
	glSpecializeShader(vertexShader, "main", 0, nullptr, nullptr);

	int compiled = 0;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compiled);
	if (compiled == GL_FALSE)
	{
		int maxLength = 0;
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

		std::string infoLog; infoLog.resize(maxLength);
		glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

		LVN_CORE_ERROR("[opengl] failed to load vertex shader module from binary file, id: (%u), error: %s", vertexShader, infoLog.c_str());
		glDeleteShader(vertexShader);
		return Lvn_Result_Failure;
	}

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderBinary(1, &fragmentShader, GL_SHADER_BINARY_FORMAT_SPIR_V, fragbin.data(), fragbin.size());
	glSpecializeShader(fragmentShader, "main", 0, nullptr, nullptr);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compiled);
	if (compiled == GL_FALSE)
	{
		int maxLength = 0;
		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

		std::string infoLog; infoLog.resize(maxLength);
		glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

		LVN_CORE_ERROR("[opengl] failed to load fragment shader module from binary file, id: (%u), error: %s", vertexShader, infoLog.c_str());
		glDeleteShader(fragmentShader);
		glDeleteShader(vertexShader);
		return Lvn_Result_Failure;
	}

	return Lvn_Result_Success;
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
		LVN_CORE_ERROR("[opengl] failed to create shader program (id:%u) when creating pipeline (%p)", shaderProgram, pipeline);
		return Lvn_Result_Failure;
	}

	pipeline->id = shaderProgram;
	pipeline->nativePipeline = lvn::memAlloc(sizeof(OglPipelineEnums));

	OglPipelineEnums* pipelineEnums = static_cast<OglPipelineEnums*>(pipeline->nativePipeline);

	pipelineEnums->enableDepth = createInfo->pipelineSpecification->depthstencil.enableDepth;

	return Lvn_Result_Success;
}

LvnResult oglsImplCreateFrameBuffer(LvnFrameBuffer* frameBuffer, LvnFrameBufferCreateInfo* createInfo)
{
	frameBuffer->frameBufferData = lvn::memAlloc(sizeof(OglFramebufferData));
	OglFramebufferData* frameBufferData = static_cast<OglFramebufferData*>(frameBuffer->frameBufferData);

	frameBufferData->width = createInfo->width;
	frameBufferData->height = createInfo->height;
	frameBufferData->textureFilter = createInfo->textureFilter;
	frameBufferData->textureMode = createInfo->textureMode;
	frameBufferData->multisampling = createInfo->sampleCount != Lvn_SampleCount_1_Bit;
	frameBufferData->hasDepth = createInfo->depthAttachment != nullptr;
	frameBufferData->sampleCount = createInfo->sampleCount;
	frameBufferData->colorAttachmentSpecifications = LvnVector(createInfo->pColorAttachments, createInfo->colorAttachmentCount);
	if (frameBufferData->hasDepth) { frameBufferData->depthAttachmentSpecification = *createInfo->depthAttachment; }

	if (ogls::updateFrameBuffer(frameBufferData) == Lvn_Result_Failure)
	{
		LVN_CORE_ERROR("[opengl] failed to update framebuffer on framebuffer creation (%p)", frameBuffer);
		return Lvn_Result_Failure;
	}

	return Lvn_Result_Success;
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
		LVN_CORE_ERROR("[opengl] last error check occurance when creating [vertex] buffer, id: %u, size: %u", vbo, createInfo->vertexBufferSize);
		return Lvn_Result_Failure;
	}

	// index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, createInfo->indexBufferSize, createInfo->pIndices, dynamicIndex ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	if (ogls::checkErrorCode() == Lvn_Result_Failure)
	{
		LVN_CORE_ERROR("[opengl] last error check occurance when creating [index] buffer, id: %u, size: %u", ibo, createInfo->indexBufferSize);
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
		LVN_CORE_ERROR("[opengl] last error check occurance when creating texture, id: %u, (w:%u,h%u), image data: %p", id, createInfo->imageData.width, createInfo->imageData.height, createInfo->imageData.pixels.data());
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
				LVN_CORE_ERROR("[opengl] invalid texture channel format (%u) when creating texture for cubemap (%p)", texImages[i]->channels, cubemap);
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
	lvn::memFree(pipeline->nativePipeline);

	glDeleteProgram(pipeline->id);
}

void oglsImplDestroyFrameBuffer(LvnFrameBuffer* frameBuffer)
{
	OglFramebufferData* frameBufferData = static_cast<OglFramebufferData*>(frameBuffer->frameBufferData);

	glDeleteFramebuffers(1, &frameBufferData->id);
	glDeleteTextures(static_cast<GLsizei>(frameBufferData->colorAttachments.size()), frameBufferData->colorAttachments.data());
	glDeleteTextures(1, &frameBufferData->depthAttachment);

	if (frameBufferData->multisampling)
	{
		glDeleteFramebuffers(1, &frameBufferData->msaaId);
		glDeleteTextures(static_cast<GLsizei>(frameBufferData->msaaColorAttachments.size()), frameBufferData->msaaColorAttachments.data());
		glDeleteTextures(1, &frameBufferData->msaaDepthAttachment);
	}

	lvn::memFree(frameBuffer->frameBufferData);
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
	OglPipelineEnums* pipelineEnums = static_cast<OglPipelineEnums*>(pipeline->nativePipeline);

	if (pipelineEnums->enableDepth) { glEnable(GL_DEPTH_TEST); }
	else { glDisable(GL_DEPTH_TEST); }

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
	OglFramebufferData* frameBufferData = static_cast<OglFramebufferData*>(frameBuffer->frameBufferData);

	glViewport(frameBufferData->x, frameBufferData->y, frameBufferData->width, frameBufferData->height);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferData->id);
}

void oglsImplRenderCmdEndFrameBuffer(LvnWindow* window, LvnFrameBuffer* frameBuffer)
{
	OglFramebufferData* frameBufferData = static_cast<OglFramebufferData*>(frameBuffer->frameBufferData);

	if (frameBufferData->multisampling)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBufferData->id);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferData->msaaId);
		glBlitFramebuffer(0, 0, frameBufferData->width, frameBufferData->height, 0, 0, frameBufferData->width, frameBufferData->height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
		LVN_CORE_WARN("[opengl] bound texture unit slots in descriptor set (%p) exceed the maximum texture unit slots that can be bound to a shader, bound texture units: (%d), maximum texture unit slots: (%d)", descriptorSet, texCount, oglBackends->maxTextureUnitSlots);
	}
}

LvnTexture* oglsImplFrameBufferGetImage(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex)
{
	OglFramebufferData* frameBufferData = static_cast<OglFramebufferData*>(frameBuffer->frameBufferData);
	LVN_CORE_ASSERT(attachmentIndex < frameBufferData->colorAttachmentTextures.size(), "attachment index out of range, cannot have an attachment index (%u) greater or equal to the total attachment count (%u) within framebuffer (%p)", attachmentIndex, frameBufferData->colorAttachmentTextures.size(), frameBuffer);

	return &frameBufferData->colorAttachmentTextures[attachmentIndex];
}

LvnRenderPass* oglsImplFrameBufferGetRenderPass(LvnFrameBuffer* frameBuffer)
{
	return nullptr;
}

void oglsImplFrameBufferResize(LvnFrameBuffer* frameBuffer, uint32_t width, uint32_t height)
{
	OglFramebufferData* frameBufferData = static_cast<OglFramebufferData*>(frameBuffer->frameBufferData);

	frameBufferData->width = width;
	frameBufferData->height = height;

	glDeleteFramebuffers(1, &frameBufferData->id);
	glDeleteTextures(static_cast<GLsizei>(frameBufferData->colorAttachments.size()), frameBufferData->colorAttachments.data());
	glDeleteTextures(1, &frameBufferData->depthAttachment);

	if (frameBufferData->multisampling)
	{
		glDeleteFramebuffers(1, &frameBufferData->msaaId);
		glDeleteTextures(static_cast<GLsizei>(frameBufferData->msaaColorAttachments.size()), frameBufferData->msaaColorAttachments.data());
		glDeleteTextures(1, &frameBufferData->msaaDepthAttachment);
	}

	frameBufferData->colorAttachments.clear();
	frameBufferData->msaaColorAttachments.clear();
	frameBufferData->depthAttachment = 0;
	frameBufferData->msaaDepthAttachment = 0;

	if (ogls::updateFrameBuffer(frameBufferData) == Lvn_Result_Failure)
	{
		LVN_CORE_ERROR("[opengl] failed to update framebuffer width and height on framebuffer recreation (%p)", frameBuffer);
	}
}

void oglsImplFrameBufferSetClearColor(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex, float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void setOglWindowContextValues()
{
	OglBackends* oglBackends = s_OglBackends;

	if (oglBackends->framebufferColorFormatSrgb)
		glEnable(GL_FRAMEBUFFER_SRGB);
}

} /* namespace lvn */
