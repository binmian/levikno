#include "lvn_opengl.h"
#include "levikno.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>


namespace lvn
{

static OglBackends* s_OglBackends = nullptr;

namespace ogls
{
	static LvnResult          checkErrorCode();
	static void GLAPIENTRY    debugCallback( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam );
	static LvnResult          checkShaderError(uint32_t shader, GLenum type, const char* shaderSrc);;
	static uint32_t           getVertexAttributeSizeEnum(LvnVertexDataType type);
	static GLenum             getVertexAttributeFormatEnum(LvnVertexDataType type);
	static GLenum             getTextureFilterEnum(LvnTextureFilter filter);
	static GLenum             getTextureWrapModeEnum(LvnTextureMode mode);
	static uint32_t           getSampleCountEnum(LvnSampleCount samples);
	static GLenum             getColorFormat(LvnColorImageFormat texFormat);
	static GLenum             getDataFormat(LvnColorImageFormat texFormat);
	static void               getDepthFormat(LvnDepthImageFormat texFormat, GLenum* format, GLenum* attachmentType);
	static GLenum             getCompareOpEnum(LvnCompareOperation compareOp);
	static GLenum             getTopologyTypeEnum(LvnTopologyType type);
	static GLenum             getBlendFactorType(LvnColorBlendFactor factor);
	static GLenum             getCullFaceModeEnum(LvnCullFaceMode mode);
	static GLenum             getCullFrontFaceEnum(LvnCullFrontFace frontFace);
	static GLenum             getUniformBufferTypeEnum(LvnDescriptorType type);
	static LvnResult          updateFrameBuffer(OglFramebufferData* frameBufferData);

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

	static void GLAPIENTRY debugCallback( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam )
	{
		LVN_CORE_ERROR("opengl callback: %s type = 0x%x, severity = 0x%x, message = %s\n", (type == GL_DEBUG_TYPE_ERROR ? "opengl error" : "" ), type, severity, message );
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
				glGetProgramInfoLog(shader, 1024, 0, infoLog);
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
				LVN_CORE_WARN("unknown vertex attribute format type enum (%d)", type);
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
			case Lvn_ColorImageFormat_None: { return GL_NONE; }
			case Lvn_ColorImageFormat_RedInt: { return GL_R32I; }
			case Lvn_ColorImageFormat_RGB: { return GL_RGB; }
			case Lvn_ColorImageFormat_RGBA: { return GL_RGBA; }
			case Lvn_ColorImageFormat_RGBA8: { return GL_RGBA8; }
			case Lvn_ColorImageFormat_RGBA16F: { return GL_RGBA16F; }
			case Lvn_ColorImageFormat_RGBA32F: { return GL_RGBA32F; }

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
			case Lvn_ColorImageFormat_None: { return GL_NONE; }
			case Lvn_ColorImageFormat_RedInt: { return GL_RED; }
			case Lvn_ColorImageFormat_RGB: { return GL_RGB; }
			case Lvn_ColorImageFormat_RGBA: { return GL_RGBA; }
			case Lvn_ColorImageFormat_RGBA8: { return GL_RGBA; }
			case Lvn_ColorImageFormat_RGBA16F: { return GL_RGBA; }
			case Lvn_ColorImageFormat_RGBA32F: { return GL_RGBA; }

			default:
			{
				LVN_CORE_WARN("invalid color data image format, setting data image format to GL_RGBA");
				return GL_RGBA;
			}
		}
	}

	static void getDepthFormat(LvnDepthImageFormat texFormat, GLenum* format, GLenum* attachmentType)
	{
		switch (texFormat)
		{
			case Lvn_DepthImageFormat_Depth16: { *format = GL_DEPTH_COMPONENT16; *attachmentType = GL_DEPTH_ATTACHMENT; break; }
			case Lvn_DepthImageFormat_Depth32: { *format = GL_DEPTH_COMPONENT32F; *attachmentType = GL_DEPTH_ATTACHMENT; break; }
			case Lvn_DepthImageFormat_Depth24Stencil8: { *format = GL_DEPTH24_STENCIL8; *attachmentType = GL_DEPTH_STENCIL_ATTACHMENT; break; }
			case Lvn_DepthImageFormat_Depth32Stencil8: { *format = GL_DEPTH32F_STENCIL8; *attachmentType = GL_DEPTH_STENCIL_ATTACHMENT; break; }
		}
	}

	static GLenum getCompareOpEnum(LvnCompareOperation compareOp)
	{
		switch (compareOp)
		{
			case Lvn_CompareOp_Never: { return GL_NEVER; }
			case Lvn_CompareOp_Less: { return GL_LESS; }
			case Lvn_CompareOp_Equal: { return GL_EQUAL; }
			case Lvn_CompareOp_LessOrEqual: { return GL_LEQUAL; }
			case Lvn_CompareOp_Greater: { return GL_GREATER; }
			case Lvn_CompareOp_NotEqual: { return GL_NOTEQUAL; }
			case Lvn_CompareOp_GreaterOrEqual: { return GL_GEQUAL; }
			case Lvn_CompareOp_Always: { return GL_ALWAYS; }

			default:
			{
				LVN_CORE_WARN("invalid compare op enum, setting topology type to GL_NEVER");
				return GL_NEVER;
			}
		}
	}

	static GLenum getTopologyTypeEnum(LvnTopologyType type)
	{
		switch (type)
		{
			case Lvn_TopologyType_Point: { return GL_POINTS; }
			case Lvn_TopologyType_Line: { return GL_LINES; }
			case Lvn_TopologyType_LineStrip: { return GL_LINE_STRIP; }
			case Lvn_TopologyType_Triangle: { return GL_TRIANGLES; }
			case Lvn_TopologyType_TriangleStrip: { return GL_TRIANGLE_STRIP; }

			default:
			{
				LVN_CORE_WARN("invalid topology type enum, setting topology type to GL_TRIANGLES");
				return GL_TRIANGLES;
			}
		}
	}

	static GLenum getBlendFactorType(LvnColorBlendFactor factor)
	{
		switch (factor)
		{
			case Lvn_ColorBlendFactor_Zero: { return GL_ZERO; }
			case Lvn_ColorBlendFactor_One: { return GL_ONE; }
			case Lvn_ColorBlendFactor_SrcColor: { return GL_SRC_COLOR; }
			case Lvn_ColorBlendFactor_OneMinusSrcColor: { return GL_ONE_MINUS_SRC_COLOR; }
			case Lvn_ColorBlendFactor_DstColor: { return GL_DST_COLOR; }
			case Lvn_ColorBlendFactor_OneMinusDstColor: { return GL_ONE_MINUS_DST_COLOR; }
			case Lvn_ColorBlendFactor_SrcAlpha: { return GL_SRC_ALPHA; }
			case Lvn_ColorBlendFactor_OneMinusSrcAlpha: { return GL_ONE_MINUS_SRC_ALPHA; }
			case Lvn_ColorBlendFactor_DstAlpha: { return GL_DST_ALPHA; }
			case Lvn_ColorBlendFactor_OneMinusDstAlpha: { return GL_ONE_MINUS_DST_ALPHA; }
			case Lvn_ColorBlendFactor_ConstantColor: { return GL_CONSTANT_COLOR; }
			case Lvn_ColorBlendFactor_OneMinusConstantColor: { return GL_ONE_MINUS_CONSTANT_COLOR; }
			case Lvn_ColorBlendFactor_ConstantAlpha: { return GL_CONSTANT_ALPHA; }
			case Lvn_ColorBlendFactor_OneMinusConstantAlpha: { return GL_ONE_MINUS_CONSTANT_ALPHA; }
			case Lvn_ColorBlendFactor_SrcAlphaSaturate: { return GL_SRC_ALPHA_SATURATE; }
			case Lvn_ColorBlendFactor_Src1Color: { return GL_SRC1_COLOR; }
			case Lvn_ColorBlendFactor_OneMinusSrc1Color: { return GL_ONE_MINUS_SRC1_COLOR; }
			case Lvn_ColorBlendFactor_Src1_Alpha: { return GL_SRC1_ALPHA; }
			case Lvn_ColorBlendFactor_OneMinusSrc1Alpha: { return GL_ONE_MINUS_SRC1_ALPHA; }

			default:
			{
				LVN_CORE_ERROR("invalid blend factor type enum, setting blend factor to GL_ZERO");
				return GL_ZERO;
			}
		}
	}

	static GLenum getCullFaceModeEnum(LvnCullFaceMode mode)
	{
		switch (mode)
		{
			case Lvn_CullFaceMode_Disable: { return GL_NONE; }
			case Lvn_CullFaceMode_Front: { return GL_FRONT; }
			case Lvn_CullFaceMode_Back: { return GL_BACK; }
			case Lvn_CullFaceMode_Both: { return GL_FRONT_AND_BACK; }

			default:
			{
				LVN_CORE_ERROR("invalid cull face mode enum, setting cull face mode to GL_NONE");
				return GL_NONE;
			}
		}
	}

	static GLenum getCullFrontFaceEnum(LvnCullFrontFace frontFace)
	{
		switch (frontFace)
		{
			case Lvn_CullFrontFace_Clockwise: { return GL_CW; }
			case Lvn_CullFrontFace_CounterClockwise: { return GL_CCW; }
			default:
			{
				LVN_CORE_ERROR("invalid cull front face enum, setting cull front face to GL_CW (clockwise)");
				return GL_CW;
			}
		}
	}

	static GLenum getUniformBufferTypeEnum(LvnDescriptorType type)
	{
		switch (type)
		{
			case Lvn_DescriptorType_UniformBuffer: { return GL_UNIFORM_BUFFER; }
			case Lvn_DescriptorType_StorageBuffer: { return GL_SHADER_STORAGE_BUFFER; }
			default:
			{
				LVN_CORE_ERROR("invalid uniform buffer type, setting buffer type to GL_UNIFORM_BUFFER");
				return GL_UNIFORM_BUFFER;
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
			ogls::getDepthFormat(frameBufferData->depthAttachmentSpecification.format, &format, &attachmentType);

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
				ogls::getDepthFormat(frameBufferData->depthAttachmentSpecification.format, &format, &attachmentType);

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
	s_OglBackends->defaultOglPipelineSpecification = lvn::configPipelineSpecificationInit();
	s_OglBackends->maxTextureUnitSlots = 32;
	s_OglBackends->framebufferColorFormatSrgb = graphicsContext->frameBufferColorFormat == Lvn_TextureFormat_Srgb ? true : false;


	// NOTE: opengl does not support any enumerated physical devices so we just create a dummy device
	LvnPhysicalDeviceProperties props{};
	props.name = s_OglBackends->deviceName;
	props.type = Lvn_PhysicalDeviceType_Other;
	props.apiVersion = (((uint32_t)(s_OglBackends->versionMajor)) << 22) | (((uint32_t)(s_OglBackends->versionMinor)) << 12);
	props.driverVersion = 0;
	props.vendorID = 0;

	LvnPhysicalDevice physicalDevice{};
	physicalDevice.physicalDevice = nullptr;
	physicalDevice.properties = props;

	s_OglBackends->physicalDevice = physicalDevice;


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

	// set error callback
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(ogls::debugCallback, 0);


	// bind function pointers
	graphicsContext->getPhysicalDevices = oglsImplGetPhysicalDevices;
	graphicsContext->checkPhysicalDeviceSupport = oglsImplCheckPhysicalDeviceSupport;
	graphicsContext->setPhysicalDevice = oglsImplSetPhysicalDevice;
	graphicsContext->createShaderFromSrc = oglsImplCreateShaderFromSrc;
	graphicsContext->createShaderFromFileSrc = oglsImplCreateShaderFromFileSrc;
	graphicsContext->createShaderFromFileBin = oglsImplCreateShaderFromFileBin;
	graphicsContext->createDescriptorLayout = oglsImplCreateDescriptorLayout;
	graphicsContext->createPipeline = oglsImplCreatePipeline;
	graphicsContext->createFrameBuffer = oglsImplCreateFrameBuffer;
	graphicsContext->createBuffer = oglsImplCreateBuffer;
	graphicsContext->createUniformBuffer = oglsImplCreateUniformBuffer;
	graphicsContext->createSampler = oglsImplCreateSampler;
	graphicsContext->createTexture = oglsImplCreateTexture;
	graphicsContext->createTextureSampler = oglsImplCreateTextureSampler;
	graphicsContext->createCubemap = oglsImplCreateCubemap;
	graphicsContext->createCubemapHdr = oglsImplCreateCubemapHdr;
	
	graphicsContext->destroyShader = oglsImplDestroyShader;
	graphicsContext->destroyDescriptorLayout = oglsImplDestroyDescriptorLayout;
	graphicsContext->destroyPipeline = oglsImplDestroyPipeline;
	graphicsContext->destroyFrameBuffer = oglsImplDestroyFrameBuffer;
	graphicsContext->destroyBuffer = oglsImplDestroyBuffer;
	graphicsContext->destroyUniformBuffer = oglsImplDestroyUniformBuffer;
	graphicsContext->destroySampler = oglsImplDestroySampler;
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
	graphicsContext->allocateDescriptorSet = oglsImplAllocateDescriptorSet;
	graphicsContext->updateUniformBufferData = oglsImplUpdateUniformBufferData;
	graphicsContext->updateDescriptorSetData = oglsImplUpdateDescriptorSetData;
	graphicsContext->frameBufferGetImage = oglsImplFrameBufferGetImage;
	graphicsContext->frameBufferGetRenderPass = oglsImplFrameBufferGetRenderPass;
	graphicsContext->framebufferResize = oglsImplFrameBufferResize;
	graphicsContext->frameBufferSetClearColor = oglsImplFrameBufferSetClearColor;
	graphicsContext->findSupportedDepthImageFormat = oglsImplFindSupportedDepthImageFormat;


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

	if (physicalDeviceCount != nullptr)
		*physicalDeviceCount = 1;

	if (pPhysicalDevices == nullptr)
		return;

	*pPhysicalDevices = &oglBackends->physicalDevice;
}

LvnResult oglsImplCheckPhysicalDeviceSupport(LvnPhysicalDevice* physicalDevice)
{
	return Lvn_Result_Success;
}

LvnResult oglsImplSetPhysicalDevice(LvnPhysicalDevice* physicalDevice)
{
	return Lvn_Result_Success;
}

LvnResult oglsImplCreateShaderFromSrc(LvnShader* shader, LvnShaderCreateInfo* createInfo)
{
	uint32_t vertexShader, fragmentShader;

	const char* vertexSrc = createInfo->vertexSrc.c_str();
	const char* fragmentSrc = createInfo->fragmentSrc.c_str();

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSrc, NULL);
	glCompileShader(vertexShader);
	if (ogls::checkShaderError(vertexShader, GL_VERTEX_SHADER, createInfo->vertexSrc.c_str()) != Lvn_Result_Success)
	{
		LVN_CORE_ERROR("[opengl] failed to create vertex shader module (id:%u) when creating shader (%p)", vertexShader, shader);
		return Lvn_Result_Failure;
	}

	shader->vertexShaderId = vertexShader;

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSrc, NULL);
	glCompileShader(fragmentShader);
	if (ogls::checkShaderError(fragmentShader, GL_FRAGMENT_SHADER, createInfo->fragmentSrc.c_str()) != Lvn_Result_Success)
	{
		LVN_CORE_ERROR("[opengl] failed to create fragment shader module (id:%u) when creating shader (%p)", fragmentShader, shader);
		return Lvn_Result_Failure;
	}

	shader->fragmentShaderId = fragmentShader;

	return Lvn_Result_Success;
}

LvnResult oglsImplCreateShaderFromFileSrc(LvnShader* shader, LvnShaderCreateInfo* createInfo)
{
	std::string fileVertSrc = lvn::loadFileSrc(createInfo->vertexSrc.c_str());
	std::string fileFragSrc = lvn::loadFileSrc(createInfo->fragmentSrc.c_str());

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
	LvnData<uint8_t> vertbin = lvn::loadFileSrcBin(createInfo->vertexSrc.c_str());
	LvnData<uint8_t> fragbin = lvn::loadFileSrcBin(createInfo->fragmentSrc.c_str());

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
	descriptorLayout->descriptorLayout = nullptr;
	descriptorLayout->descriptorPool = new OglDescriptorSet[descriptorLayout->descriptorSets.size()];

	for (uint64_t i = 0; i < descriptorLayout->descriptorSets.size(); i++)
	{
		OglDescriptorSet* descriptorSet = &static_cast<OglDescriptorSet*>(descriptorLayout->descriptorPool)[i];

		for (uint32_t j = 0; j < createInfo->descriptorBindingCount; j++)
		{
			LvnDescriptorType descriptorType = createInfo->pDescriptorBindings[j].descriptorType;

			if (descriptorType == Lvn_DescriptorType_UniformBuffer || descriptorType == Lvn_DescriptorType_StorageBuffer)
			{
				OglDescriptorBinding descriptorBinding{};
				descriptorBinding.type = descriptorType;
				descriptorBinding.binding = createInfo->pDescriptorBindings[j].binding;
				descriptorSet->uniformBuffers.push_back(descriptorBinding);
			}
			else if (descriptorType == Lvn_DescriptorType_ImageSampler)
			{
				OglDescriptorBinding descriptorBinding{};
				descriptorBinding.type = descriptorType;
				descriptorBinding.binding = createInfo->pDescriptorBindings[j].binding;
				descriptorBinding.count = createInfo->pDescriptorBindings[j].descriptorCount;
				descriptorSet->textures.push_back(descriptorBinding);
			}
		}
	}

	return Lvn_Result_Success;
}

LvnResult oglsImplAllocateDescriptorSet(LvnDescriptorSet* descriptorSet, LvnDescriptorLayout* descriptorLayout)
{
	descriptorSet->singleSet = &static_cast<OglDescriptorSet*>(descriptorLayout->descriptorPool)[descriptorLayout->descriptorSetIndex - 1]; // NOTE: set index minus one due to index shifted before
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
	pipeline->nativePipeline = new OglPipelineEnums();

	OglPipelineEnums* pipelineEnums = static_cast<OglPipelineEnums*>(pipeline->nativePipeline);

	pipelineEnums->enableDepth = createInfo->pipelineSpecification->depthstencil.enableDepth;
	pipelineEnums->enableBlending = createInfo->pipelineSpecification->colorBlend.colorBlendAttachmentCount > 0;
	pipelineEnums->enableCulling = createInfo->pipelineSpecification->rasterizer.cullMode != Lvn_CullFaceMode_Disable;

	pipelineEnums->depthCompareOp = ogls::getCompareOpEnum(createInfo->pipelineSpecification->depthstencil.depthOpCompare);
	pipelineEnums->topologyType = ogls::getTopologyTypeEnum(createInfo->pipelineSpecification->inputAssembly.topology);

	if (pipelineEnums->enableBlending)
	{
		pipelineEnums->srcBlendFactor = ogls::getBlendFactorType(createInfo->pipelineSpecification->colorBlend.pColorBlendAttachments[0].srcColorBlendFactor);
		pipelineEnums->dstBlendFactor = ogls::getBlendFactorType(createInfo->pipelineSpecification->colorBlend.pColorBlendAttachments[0].dstColorBlendFactor);
	}

	if (pipelineEnums->enableCulling)
	{
		pipelineEnums->cullMode = ogls::getCullFaceModeEnum(createInfo->pipelineSpecification->rasterizer.cullMode);
		pipelineEnums->frontFace = ogls::getCullFrontFaceEnum(createInfo->pipelineSpecification->rasterizer.frontFace);
	}

	return Lvn_Result_Success;
}

LvnResult oglsImplCreateFrameBuffer(LvnFrameBuffer* frameBuffer, LvnFrameBufferCreateInfo* createInfo)
{
	frameBuffer->frameBufferData = new OglFramebufferData();
	OglFramebufferData* frameBufferData = static_cast<OglFramebufferData*>(frameBuffer->frameBufferData);

	frameBufferData->width = createInfo->width;
	frameBufferData->height = createInfo->height;
	frameBufferData->textureFilter = createInfo->textureFilter;
	frameBufferData->textureMode = createInfo->textureMode;
	frameBufferData->multisampling = createInfo->sampleCount != Lvn_SampleCount_1_Bit;
	frameBufferData->hasDepth = createInfo->depthAttachment != nullptr;
	frameBufferData->sampleCount = createInfo->sampleCount;
	frameBufferData->colorAttachmentSpecifications = std::vector(createInfo->pColorAttachments, createInfo->pColorAttachments + createInfo->colorAttachmentCount);
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
	glGenVertexArrays(1, &buffer->id);
	glGenBuffers(1, &buffer->vboId);
	glGenBuffers(1, &buffer->iboId);

	glBindVertexArray(buffer->id);

	bool dynamicVertex = false, dynamicIndex = false;

	if (createInfo->type & Lvn_BufferType_DynamicVertex)
		dynamicVertex = true;
	if (createInfo->type & Lvn_BufferType_DynamicIndex)
		dynamicIndex = true;

	// vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffer->vboId);
	glBufferData(GL_ARRAY_BUFFER, createInfo->vertexBufferSize, createInfo->pVertices, dynamicVertex ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	if (ogls::checkErrorCode() == Lvn_Result_Failure)
	{
		LVN_CORE_ERROR("[opengl] last error check occurance when creating [vertex] buffer, id: %u, size: %u", buffer->vboId, createInfo->vertexBufferSize);
		return Lvn_Result_Failure;
	}

	// index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->iboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, createInfo->indexBufferSize, createInfo->pIndices, dynamicIndex ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	if (ogls::checkErrorCode() == Lvn_Result_Failure)
	{
		LVN_CORE_ERROR("[opengl] last error check occurance when creating [index] buffer, id: %u, size: %u", buffer->iboId, createInfo->indexBufferSize);
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

	buffer->type = createInfo->type;
	buffer->vertexBuffer = nullptr;
	buffer->vertexBufferSize = createInfo->vertexBufferSize;
	buffer->vertexBufferMemory = nullptr;
	buffer->indexBuffer = nullptr;
	buffer->indexBufferSize = createInfo->indexBufferSize;
	buffer->indexBufferMemory = nullptr;
	buffer->indexOffset = 0;

	return Lvn_Result_Success;
}

LvnResult oglsImplCreateUniformBuffer(LvnUniformBuffer* uniformBuffer, LvnUniformBufferCreateInfo* createInfo)
{
	uint32_t id;
	glCreateBuffers(1, &id);
	glNamedBufferData(id, createInfo->size, nullptr, GL_DYNAMIC_DRAW);

	uniformBuffer->id = id;
	uniformBuffer->size = createInfo->size;
	
	return Lvn_Result_Success;
}

LvnResult oglsImplCreateSampler(LvnSampler* sampler, LvnSamplerCreateInfo* createInfo)
{
	OglSampler* oglSampler = new OglSampler();

	oglSampler->minFilter = createInfo->minFilter;
	oglSampler->magFilter = createInfo->magFilter;
	oglSampler->wrapS = createInfo->wrapS;
	oglSampler->wrapT = createInfo->wrapT;

	sampler->sampler = oglSampler;

	return Lvn_Result_Success;
}

LvnResult oglsImplCreateTexture(LvnTexture* texture, LvnTextureCreateInfo* createInfo)
{
	GLenum format = createInfo->format == Lvn_TextureFormat_Unorm ? GL_RGB8 : GL_SRGB8;
	GLenum internalFormat = GL_RGB;
	switch (createInfo->imageData.channels)
	{
		case 1: { internalFormat = createInfo->format == Lvn_TextureFormat_Unorm ? GL_R8 : GL_R8; format = GL_RED; break; }
		case 2: { internalFormat = createInfo->format == Lvn_TextureFormat_Unorm ? GL_RG8 : GL_RG8; format = GL_RG; break; }
		case 3: { internalFormat = createInfo->format == Lvn_TextureFormat_Unorm ? GL_RGB8 : GL_SRGB8; format = GL_RGB; break; }
		case 4: { internalFormat = createInfo->format == Lvn_TextureFormat_Unorm ? GL_RGBA8 : GL_SRGB8_ALPHA8; format = GL_RGBA; break; }
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	uint32_t id;
	glCreateTextures(GL_TEXTURE_2D, 1, &id);
	glTextureStorage2D(id, 1, internalFormat, createInfo->imageData.width, createInfo->imageData.height);

	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ogls::getTextureWrapModeEnum(createInfo->wrapS));
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ogls::getTextureWrapModeEnum(createInfo->wrapT));
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, ogls::getTextureWrapModeEnum(createInfo->wrapT));
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ogls::getTextureFilterEnum(createInfo->minFilter));
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ogls::getTextureFilterEnum(createInfo->magFilter));

	glTextureSubImage2D(id, 0, 0, 0, createInfo->imageData.width, createInfo->imageData.height, format, GL_UNSIGNED_BYTE, createInfo->imageData.pixels.data());

	glGenerateMipmap(GL_TEXTURE_2D);

	if (ogls::checkErrorCode() == Lvn_Result_Failure)
	{
		LVN_CORE_ERROR("[opengl] last error check occurance when creating texture, id: %u, (w:%u,h%u), image data: %p", id, createInfo->imageData.width, createInfo->imageData.height, createInfo->imageData.pixels.data());
		return Lvn_Result_Failure;
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	texture->id = id;
	texture->width = createInfo->imageData.width;
	texture->height = createInfo->imageData.height;
	texture->seperateSampler = false;

	return Lvn_Result_Success;
}

LvnResult oglsImplCreateTextureSampler(LvnTexture* texture, LvnTextureSamplerCreateInfo* createInfo)
{
	OglSampler* sampler = static_cast<OglSampler*>(createInfo->sampler->sampler);

	GLenum format = createInfo->format == Lvn_TextureFormat_Unorm ? GL_RGB8 : GL_SRGB8;
	GLenum internalFormat = GL_RGB;
	switch (createInfo->imageData.channels)
	{
		case 1: { internalFormat = createInfo->format == Lvn_TextureFormat_Unorm ? GL_R8 : GL_R8; format = GL_RED; break; }
		case 2: { internalFormat = createInfo->format == Lvn_TextureFormat_Unorm ? GL_RG8 : GL_RG8; format = GL_RG; break; }
		case 3: { internalFormat = createInfo->format == Lvn_TextureFormat_Unorm ? GL_RGB8 : GL_SRGB8; format = GL_RGB; break; }
		case 4: { internalFormat = createInfo->format == Lvn_TextureFormat_Unorm ? GL_RGBA8 : GL_SRGB8_ALPHA8; format = GL_RGBA; break; }
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	uint32_t id;
	glCreateTextures(GL_TEXTURE_2D, 1, &id);
	glTextureStorage2D(id, 1, internalFormat, createInfo->imageData.width, createInfo->imageData.height);

	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ogls::getTextureWrapModeEnum(sampler->wrapS));
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ogls::getTextureWrapModeEnum(sampler->wrapT));
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, ogls::getTextureWrapModeEnum(sampler->wrapT));
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ogls::getTextureFilterEnum(sampler->minFilter));
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ogls::getTextureFilterEnum(sampler->magFilter));

	glTextureSubImage2D(id, 0, 0, 0, createInfo->imageData.width, createInfo->imageData.height, format, GL_UNSIGNED_BYTE, createInfo->imageData.pixels.data());

	glGenerateMipmap(GL_TEXTURE_2D);

	if (ogls::checkErrorCode() == Lvn_Result_Failure)
	{
		LVN_CORE_ERROR("[opengl] last error check occurance when creating texture, id: %u, (w:%u,h%u), image data: %p", id, createInfo->imageData.width, createInfo->imageData.height, createInfo->imageData.pixels.data());
		return Lvn_Result_Failure;
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	texture->id = id;
	texture->width = createInfo->imageData.width;
	texture->height = createInfo->imageData.height;
	texture->seperateSampler = true;

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

LvnResult oglsImplCreateCubemapHdr(LvnCubemap* cubemap, LvnCubemapHdrCreateInfo* createInfo)
{
	// TODO: implement cubemap hdr

	return Lvn_Result_Success;
}


void oglsImplDestroyShader(LvnShader* shader)
{
	glDeleteShader(shader->vertexShaderId);
	glDeleteShader(shader->fragmentShaderId);
}

void oglsImplDestroyDescriptorLayout(LvnDescriptorLayout* descriptorLayout)
{
	for (uint64_t i = 0; i < descriptorLayout->descriptorSets.size(); i++)
	{
		OglDescriptorSet* descriptorSet = &static_cast<OglDescriptorSet*>(descriptorLayout->descriptorPool)[i];

		for (const OglBindlessTextureBinding& bindlessTexBinding : descriptorSet->bindlessTextures)
		{
			for (const uint64_t& handle : bindlessTexBinding.textureHandles)
			{
				glMakeTextureHandleNonResidentARB(handle);
			}

			glDeleteBuffers(1, &bindlessTexBinding.ssbo);
		}
		descriptorSet->bindlessTextures.clear();
	}

	delete [] static_cast<OglDescriptorSet*>(descriptorLayout->descriptorPool);
}

void oglsImplDestroyPipeline(LvnPipeline* pipeline)
{
	OglPipelineEnums* pipelineEnums = static_cast<OglPipelineEnums*>(pipeline->nativePipeline);
	delete pipelineEnums;

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

	delete frameBufferData;
}

void oglsImplDestroyBuffer(LvnBuffer* buffer)
{
	glDeleteBuffers(1, &buffer->vboId);
	glDeleteBuffers(1, &buffer->iboId);
	glDeleteVertexArrays(1, &buffer->id);
}

void oglsImplDestroyUniformBuffer(LvnUniformBuffer* uniformBuffer)
{
	glDeleteBuffers(1, &uniformBuffer->id);
}

void oglsImplDestroySampler(LvnSampler* sampler)
{
	OglSampler* oglSampler = static_cast<OglSampler*>(sampler->sampler);
	delete oglSampler;
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
	glDrawArrays(window->topologyTypeEnum, 0, vertexCount);
}

void oglsImplRenderCmdDrawIndexed(LvnWindow* window, uint32_t indexCount)
{
	glDrawElements(window->topologyTypeEnum, indexCount, GL_UNSIGNED_INT, 0);
}

void oglsImplRenderCmdDrawInstanced(LvnWindow* window, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstInstance)
{
	glDrawArraysInstancedBaseInstance(window->topologyTypeEnum, 0, vertexCount, instanceCount, firstInstance);
}

void oglsImplRenderCmdDrawIndexedInstanced(LvnWindow* window, uint32_t indexCount, uint32_t instanceCount, uint32_t firstInstance)
{
	glDrawElementsInstancedBaseInstance(window->topologyTypeEnum, indexCount, GL_UNSIGNED_INT, 0, instanceCount, firstInstance);
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

	// depth
	if (pipelineEnums->enableDepth)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(pipelineEnums->depthCompareOp);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}

	// color blend
	if (pipelineEnums->enableBlending)
	{
		glEnable(GL_BLEND);
		glBlendFunc(pipelineEnums->srcBlendFactor, pipelineEnums->dstBlendFactor);
	}
	else
	{
		glDisable(GL_BLEND);
	}

	if (pipelineEnums->enableCulling)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(pipelineEnums->cullMode);
		glFrontFace(pipelineEnums->frontFace);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}

	glUseProgram(pipeline->id);

	window->topologyTypeEnum = pipelineEnums->topologyType;
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

		// uniform/storage buffers
		for (uint32_t j = 0; j < descriptorSetPtr->uniformBuffers.size(); j++)
		{
			glBindBufferRange(ogls::getUniformBufferTypeEnum(descriptorSetPtr->uniformBuffers[j].type),
			    descriptorSetPtr->uniformBuffers[j].binding,
			    descriptorSetPtr->uniformBuffers[j].id,
			    static_cast<GLintptr>(descriptorSetPtr->uniformBuffers[j].offset),
			    static_cast<GLsizeiptr>(descriptorSetPtr->uniformBuffers[j].range));
		}

		// textures
		for (uint32_t j = 0; j < descriptorSetPtr->textures.size(); j++)
		{
			if (texCount >= oglBackends->maxTextureUnitSlots)
			{
				LVN_CORE_WARN("maximum texture unit slots exceeded, cannot bind more texture unit slots to one shader pipeline. Max slots: %u", oglBackends->maxTextureUnitSlots);
				return;
			}

			glBindTextureUnit(descriptorSetPtr->textures[j].binding, descriptorSetPtr->textures[j].id);
			texCount++;
		}

		// bindless textures
		for (const OglBindlessTextureBinding& bindlessTextureBinding : descriptorSetPtr->bindlessTextures)
		{
			for (const uint64_t& handle : bindlessTextureBinding.textureHandles)
			{
				glMakeTextureHandleResidentARB(handle);
			}

			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindlessTextureBinding.binding, bindlessTextureBinding.ssbo);
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

void oglsImplBufferUpdateVertexData(LvnBuffer* buffer, void* vertices, uint64_t size, uint64_t offset)
{
	glNamedBufferSubData(buffer->vboId, offset, size, vertices);
}

void oglsImplBufferUpdateIndexData(LvnBuffer* buffer, uint32_t* indices, uint64_t size, uint64_t offset)
{
	glNamedBufferSubData(buffer->iboId, offset, size, indices);
}

void oglsImplBufferResizeVertexBuffer(LvnBuffer* buffer, uint64_t size)
{
	LVN_CORE_ASSERT(buffer->type & Lvn_BufferType_DynamicVertex, "[opengl] cannot change vertex data of buffer that does not have dynamic vertex buffer type set (Lvn_BufferType_DynamicVertex)");

	glNamedBufferData(buffer->vboId, size, nullptr, GL_DYNAMIC_DRAW);

	if (ogls::checkErrorCode() == Lvn_Result_Failure)
	{
		LVN_CORE_ERROR("[opengl] last error check occurance when resizing [vertex] buffer, id: %u", buffer->vboId);
	}
}

void oglsImplBufferResizeIndexBuffer(LvnBuffer* buffer, uint64_t size)
{
	LVN_CORE_ASSERT(buffer->type & Lvn_BufferType_DynamicIndex, "[opengl] cannot change index data of buffer that does not have dynamic index buffer type set (Lvn_BufferType_DynamicIndex)");

	glNamedBufferData(buffer->iboId, size, nullptr, GL_DYNAMIC_DRAW);

	if (ogls::checkErrorCode() == Lvn_Result_Failure)
	{
		LVN_CORE_ERROR("[opengl] last error check occurance when resizing [index] buffer, id: %u", buffer->iboId);
	}
}

void oglsImplUpdateUniformBufferData(LvnUniformBuffer* uniformBuffer, void* data, uint64_t size, uint64_t offset)
{
	glNamedBufferSubData(uniformBuffer->id, offset, size, data);
}

void oglsImplUpdateDescriptorSetData(LvnDescriptorSet* descriptorSet, LvnDescriptorUpdateInfo* pUpdateInfo, uint32_t count)
{
	OglBackends* oglBackends = s_OglBackends;
	int texCount = 0;

	OglDescriptorSet* descriptorSetPtr = static_cast<OglDescriptorSet*>(descriptorSet->singleSet);

	// clean up bindless texture on previous updates
	for (const OglBindlessTextureBinding& bindlessTexBinding : descriptorSetPtr->bindlessTextures)
	{
		for (const uint64_t& handle : bindlessTexBinding.textureHandles)
		{
			glMakeTextureHandleNonResidentARB(handle);
		}

		glDeleteBuffers(1, &bindlessTexBinding.ssbo);
	}
	descriptorSetPtr->bindlessTextures.clear();

	for (uint32_t i = 0; i < count; i++)
	{
		// uniform buffer
		if (pUpdateInfo[i].descriptorType == Lvn_DescriptorType_UniformBuffer || pUpdateInfo[i].descriptorType == Lvn_DescriptorType_StorageBuffer)
		{
			for (uint32_t j = 0; j < descriptorSetPtr->uniformBuffers.size(); j++)
			{
				if (descriptorSetPtr->uniformBuffers[j].binding == pUpdateInfo[i].binding)
				{
					descriptorSetPtr->uniformBuffers[j].id = pUpdateInfo[i].bufferInfo->buffer->id;
					descriptorSetPtr->uniformBuffers[j].range = pUpdateInfo[i].bufferInfo->range;
					descriptorSetPtr->uniformBuffers[j].offset = pUpdateInfo[i].bufferInfo->offset;
					break;
				}
			}
		}

		// texture image
		else if (pUpdateInfo[i].descriptorType == Lvn_DescriptorType_ImageSampler)
		{
			// textures
			for (uint32_t j = 0; j < descriptorSetPtr->textures.size(); j++)
			{
				if (descriptorSetPtr->textures[j].binding == pUpdateInfo[i].binding)
				{
					descriptorSetPtr->textures[j].id = pUpdateInfo[i].pTextureInfos[0]->id;
					texCount++;
					break;
				}
			}
		}

		// bindless textures; note they are created/added during descriptor update
		else if (pUpdateInfo[i].descriptorType == Lvn_DescriptorType_ImageSamplerBindless)
		{
			OglBindlessTextureBinding bindlessTexture{};
			for (uint32_t j = 0; j < pUpdateInfo[i].descriptorCount; j++)
			{
				// iterate through the textures and get the texture handles
				uint32_t texId = pUpdateInfo[i].pTextureInfos[j]->id;
				GLuint64 handle = glGetTextureHandleARB(texId);
				if (!handle)
				{
					LVN_CORE_ERROR("[opengl] failed to get texture handle for bindless texturing, texture id: (%u)", texId);
					return;
				}
				bindlessTexture.textureHandles.push_back(handle);
			}

			glCreateBuffers(1, &bindlessTexture.ssbo);
			glNamedBufferData(bindlessTexture.ssbo, bindlessTexture.textureHandles.size() * sizeof(uint64_t), bindlessTexture.textureHandles.data(), GL_DYNAMIC_DRAW);
			bindlessTexture.binding = pUpdateInfo[i].binding;
			descriptorSetPtr->bindlessTextures.push_back(bindlessTexture);
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

LvnDepthImageFormat oglsImplFindSupportedDepthImageFormat(LvnDepthImageFormat* pDepthImageFormats, uint32_t count)
{
	return pDepthImageFormats[0];
}

void setOglWindowContextValues()
{
	OglBackends* oglBackends = s_OglBackends;

	if (oglBackends->framebufferColorFormatSrgb)
		glEnable(GL_FRAMEBUFFER_SRGB);
}

} /* namespace lvn */
