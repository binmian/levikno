#include "levikno_internal.h"

#define LVN_ARRAY_LEN(x) (sizeof(x) / sizeof(x[0]))


static const char* s_rendererDefaultVertexShaderCode = R"(
#version 460

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec4 inColor;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec4 fragColor;

layout (binding = 0) uniform ObjectBuffer
{
	mat4 matrix;
} ubo;

void main()
{
	gl_Position = ubo.matrix * vec4(inPos, 0.0, 1.0);
	fragTexCoord = inTexCoord;
	fragColor = inColor;
}

)";

static const char* s_rendererDefaultFragmentShaderCodeTypeVks = R"(
#version 460

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec4 fragColor;

layout(binding = 1) uniform sampler2D inTexture;

void main()
{
	vec3 color = vec3(texture(inTexture, fragTexCoord) * fragColor);
	outColor = vec4(color, fragColor.a);
}
)";

static const char* s_rendererDefaultFragmentShaderCodeTypeOgl = R"(
#version 460

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec4 fragColor;

uniform sampler2D inTexture;

void main()
{
	vec3 color = vec3(texture(inTexture, fragTexCoord) * fragColor);
	outColor = vec4(color, fragColor.a);
}
)";

namespace lvn
{

LvnResult rendererInit(LvnRenderer* renderer, LvnRendererCreateInfo* createInfo)
{
	if (!renderer)
	{
		LVN_CORE_ERROR("[R2D] cannot create 2d renderer, parameter renderer was nullptr");
		return Lvn_Result_Failure;
	}

	if (lvn::createWindow(&renderer->window, &createInfo->windowCreateInfo) != Lvn_Result_Success)
	{
		LVN_CORE_ERROR("[R2D] cannot create 2d renderer, failed to create window");
		return Lvn_Result_Failure;
	}

	renderer->backGroundColor = LvnVec4(0.0f, 0.0f, 0.0f, 1.0f);

	// create draw buffer
	LvnVertexAttribute drawAttributes[] =
	{
		{ 0, 0, Lvn_VertexDataType_Vec2f, 0 },
		{ 0, 1, Lvn_VertexDataType_Vec2f, (2 * sizeof(float)) },
		{ 0, 2, Lvn_VertexDataType_Vec4f, (4 * sizeof(float)) },
	};

	LvnVertexBindingDescription drawVertexBindingDescription{};
	drawVertexBindingDescription.binding = 0;
	drawVertexBindingDescription.stride = 8 * sizeof(float);

	LvnBufferCreateInfo drawBufferCreateInfo{};
	drawBufferCreateInfo.type = Lvn_BufferType_DynamicVertex | Lvn_BufferType_DynamicIndex;
	drawBufferCreateInfo.pVertexAttributes = drawAttributes;
	drawBufferCreateInfo.vertexAttributeCount = LVN_ARRAY_LEN(drawAttributes);
	drawBufferCreateInfo.pVertexBindingDescriptions = &drawVertexBindingDescription;
	drawBufferCreateInfo.vertexBindingDescriptionCount = 1;
	drawBufferCreateInfo.pVertices = nullptr;
	drawBufferCreateInfo.vertexBufferSize = createInfo->vertexBufferSize;
	drawBufferCreateInfo.pIndices = nullptr;
	drawBufferCreateInfo.indexBufferSize = createInfo->indexBufferSize;

	// create buffer
	if (lvn::createBuffer(&renderer->drawBuffer, &drawBufferCreateInfo) != Lvn_Result_Success)
	{
		LVN_CORE_ERROR("[R2D] cannot create 2d renderer, failed to create buffer");
		return Lvn_Result_Failure;
	}

	LvnShaderCreateInfo drawShaderCreateInfo{};
	drawShaderCreateInfo.vertexSrc = s_rendererDefaultVertexShaderCode;

	switch (lvn::getGraphicsApi())
	{
		case Lvn_GraphicsApi_OpenGL:
		{
			drawShaderCreateInfo.fragmentSrc = s_rendererDefaultFragmentShaderCodeTypeOgl;
			break;
		}
		case Lvn_GraphicsApi_Vulkan:
		{
			drawShaderCreateInfo.fragmentSrc = s_rendererDefaultFragmentShaderCodeTypeVks;
			break;
		}

		default:
		{
			LVN_CORE_ERROR("[R2D] cannot create 2d renderer, no graphics api selected");
			return Lvn_Result_Failure;
		}
	}

	LvnShader* drawShader;
	if (lvn::createShaderFromSrc(&drawShader, &drawShaderCreateInfo) != Lvn_Result_Success)
	{
		LVN_CORE_ERROR("[R2D] cannot create 2d renderer, failed to create draw shaders");
		return Lvn_Result_Failure;
	}


	// descriptor binding
	LvnDescriptorBinding descriptorBindingUniform{};
	descriptorBindingUniform.binding = 0;
	descriptorBindingUniform.descriptorType = Lvn_DescriptorType_UniformBuffer;
	descriptorBindingUniform.shaderStage = Lvn_ShaderStage_Vertex;
	descriptorBindingUniform.descriptorCount = 1;
	descriptorBindingUniform.maxAllocations = 1;

	LvnDescriptorBinding descriptorBindingTexture{};
	descriptorBindingTexture.binding = 1;
	descriptorBindingTexture.descriptorType = Lvn_DescriptorType_CombinedImageSampler;
	descriptorBindingTexture.shaderStage = Lvn_ShaderStage_Fragment;
	descriptorBindingTexture.descriptorCount = 1;
	descriptorBindingTexture.maxAllocations = 1;

	LvnDescriptorBinding descriptorBindings[] =
	{
		descriptorBindingUniform, descriptorBindingTexture,
	};

	// descriptor layout create info
	LvnDescriptorLayoutCreateInfo descriptorLayoutCreateInfo{};
	descriptorLayoutCreateInfo.pDescriptorBindings = descriptorBindings;
	descriptorLayoutCreateInfo.descriptorBindingCount = LVN_ARRAY_LEN(descriptorBindings);
	descriptorLayoutCreateInfo.maxSets = 1;

	// create descriptor layout and set
	if (lvn::createDescriptorLayout(&renderer->drawDescriptorLayout, &descriptorLayoutCreateInfo) != Lvn_Result_Success)
	{
		LVN_CORE_ERROR("[R2D] cannot create 2d renderer, failed to create descriptor layout");
		return Lvn_Result_Failure;
	}
	if (lvn::createDescriptorSet(&renderer->drawDescriptorSet, renderer->drawDescriptorLayout) != Lvn_Result_Success)
	{
		LVN_CORE_ERROR("[R2D] cannot create 2d renderer, failed to create descriptor set");
		return Lvn_Result_Failure;
	}


	LvnRenderPass* renderPass = lvn::windowGetRenderPass(renderer->window);
	LvnPipelineSpecification pipelineSpec = lvn::pipelineSpecificationGetConfig();

	// pipeline create info struct
	LvnPipelineCreateInfo pipelineCreateInfo{};
	pipelineCreateInfo.pipelineSpecification = &pipelineSpec;
	pipelineCreateInfo.pVertexAttributes = drawAttributes;
	pipelineCreateInfo.vertexAttributeCount = LVN_ARRAY_LEN(drawAttributes);
	pipelineCreateInfo.pVertexBindingDescriptions = &drawVertexBindingDescription;
	pipelineCreateInfo.vertexBindingDescriptionCount = 1;
	pipelineCreateInfo.pDescriptorLayouts = &renderer->drawDescriptorLayout;
	pipelineCreateInfo.descriptorLayoutCount = 1;
	pipelineCreateInfo.shader = drawShader;
	pipelineCreateInfo.renderPass = renderPass;

	// create pipeline
	if (lvn::createPipeline(&renderer->drawPipeline, &pipelineCreateInfo) != Lvn_Result_Success)
	{
		LVN_CORE_ERROR("[R2D] cannot create 2d renderer, failed to create pipeline");
		return Lvn_Result_Failure;
	}

	lvn::destroyShader(drawShader);


	// uniform buffer create info struct
	LvnUniformBufferCreateInfo uniformBufferCreateInfo{};
	uniformBufferCreateInfo.type = Lvn_BufferType_Uniform;
	uniformBufferCreateInfo.binding = 0;
	uniformBufferCreateInfo.size = sizeof(LvnRenderer::LvnRendererUniformData);

	// create uniform buffer
	lvn::createUniformBuffer(&renderer->uniformBuffer, &uniformBufferCreateInfo);


	// create default texture
	uint8_t whiteTextureData[] = { 0xff, 0xff, 0xff, 0xff };
	LvnImageData imageData;
	imageData.pixels = LvnData<uint8_t>(whiteTextureData, LVN_ARRAY_LEN(whiteTextureData));
	imageData.width = 1;
	imageData.height = 1;
	imageData.channels = 4;
	imageData.size = 4;

	LvnTextureCreateInfo textureCreateInfo{};
	textureCreateInfo.imageData = imageData;
	textureCreateInfo.minFilter = Lvn_TextureFilter_Nearest;
	textureCreateInfo.magFilter = Lvn_TextureFilter_Nearest;
	textureCreateInfo.wrapMode = Lvn_TextureMode_Repeat;
	textureCreateInfo.format = Lvn_TextureFormat_Unorm;

	lvn::createTexture(&renderer->texture, &textureCreateInfo);

	// update descriptor set
	LvnDescriptorUpdateInfo descriptorUniformUpdateInfo{};
	descriptorUniformUpdateInfo.descriptorType = Lvn_DescriptorType_UniformBuffer;
	descriptorUniformUpdateInfo.binding = 0;
	descriptorUniformUpdateInfo.descriptorCount = 1;
	descriptorUniformUpdateInfo.bufferInfo = renderer->uniformBuffer;

	LvnDescriptorUpdateInfo descriptorTextureUpdateInfo{};
	descriptorTextureUpdateInfo.descriptorType = Lvn_DescriptorType_CombinedImageSampler;
	descriptorTextureUpdateInfo.binding = 1;
	descriptorTextureUpdateInfo.descriptorCount = 1;
	descriptorTextureUpdateInfo.textureInfo = renderer->texture;

	LvnDescriptorUpdateInfo descriptorUpdateInfos[] =
	{
		descriptorUniformUpdateInfo, descriptorTextureUpdateInfo,
	};

	lvn::updateDescriptorSetData(renderer->drawDescriptorSet, descriptorUpdateInfos, LVN_ARRAY_LEN(descriptorUpdateInfos));

	return Lvn_Result_Success;
}

void rendererTerminate(LvnRenderer* renderer)
{
	if (renderer->window) { lvn::destroyWindow(renderer->window); }
	if (renderer->drawBuffer) { lvn::destroyBuffer(renderer->drawBuffer); }
	if (renderer->drawPipeline) { lvn::destroyPipeline(renderer->drawPipeline); }
	if (renderer->drawDescriptorLayout) { lvn::destroyDescriptorLayout(renderer->drawDescriptorLayout); }
	if (renderer->drawDescriptorSet) { lvn::destroyDescriptorSet(renderer->drawDescriptorSet); }
	if (renderer->texture) { lvn::destroyTexture(renderer->texture); }
	if (renderer->uniformBuffer) { lvn::destroyUniformBuffer(renderer->uniformBuffer); }
}

void rendererBeginDraw2d(LvnRenderer* renderer)
{
	lvn::windowUpdate(renderer->window);

	int width, height;
	lvn::windowGetSize(renderer->window, &width, &height);

	// update matrix
	LvnMat4 proj = lvn::ortho((float)width * -0.5f, (float)width * 0.5f, (float)height * -0.5f, (float)height * 0.5f, -1.0f, 1.0f);
	LvnMat4 view = LvnMat4(1.0f);
	LvnMat4 camera = proj * view;

	renderer->uniformData.matrix = camera;
	lvn::updateUniformBufferData(renderer->window, renderer->uniformBuffer, &renderer->uniformData, sizeof(renderer->uniformData));

	lvn::renderBeginNextFrame(renderer->window);
	lvn::renderBeginCommandRecording(renderer->window);

	lvn::renderClearColor(renderer->window, renderer->backGroundColor.r, renderer->backGroundColor.g, renderer->backGroundColor.b, renderer->backGroundColor.a);
	lvn::renderCmdBeginRenderPass(renderer->window);
}

void rendererEndDraw2d(LvnRenderer* renderer)
{
	lvn::bufferUpdateVertexData(renderer->drawBuffer, renderer->drawList.vertices(), renderer->drawList.vertex_size(), 0);
	lvn::bufferUpdateIndexData(renderer->drawBuffer, renderer->drawList.indices(), renderer->drawList.index_size(), 0);


	lvn::renderCmdBindPipeline(renderer->window, renderer->drawPipeline);
	lvn::renderCmdBindDescriptorSets(renderer->window, renderer->drawPipeline, 0, 1, &renderer->drawDescriptorSet);

	lvn::renderCmdBindVertexBuffer(renderer->window, renderer->drawBuffer);
	lvn::renderCmdBindIndexBuffer(renderer->window, renderer->drawBuffer);

	lvn::renderCmdDrawIndexed(renderer->window, renderer->drawList.index_count());

	lvn::renderCmdEndRenderPass(renderer->window);
	lvn::renderEndCommandRecording(renderer->window);
	lvn::renderDrawSubmit(renderer->window);

	renderer->drawList.clear();
}

void rendererSetBackgroundColor(LvnRenderer* renderer, float r, float g, float b, float a)
{
	renderer->backGroundColor = LvnVec4(r, g, b, a);
}

void drawTriangle(LvnRenderer* renderer, const LvnTriangle& triangle, const LvnVec4& color)
{
	float vertices[] =
	{
	//            pos (x,y)         |     uv    |             color (rgba)
		triangle.v1.x, triangle.v1.y, 0.0f, 0.0f, color.r, color.g, color.b, color.a,
		triangle.v2.x, triangle.v2.y, 1.0f, 0.0f, color.r, color.g, color.b, color.a,
		triangle.v3.x, triangle.v3.y, 0.5f, 1.0f, color.r, color.g, color.b, color.a,
	};

	uint32_t indices[] =
	{
		0, 1, 2
	};

	LvnDrawCommand drawCmd{};
	drawCmd.pVertices = vertices;
	drawCmd.vertexAttributeCount = 8;
	drawCmd.vertexCount = 3;
	drawCmd.pIndices = indices;
	drawCmd.indexCount = 3;

	renderer->drawList.push_back(drawCmd);
}

void drawRect(LvnRenderer* renderer, const LvnRect& rect, const LvnVec4& color)
{
	float vertices[] =
	{
	//                      pos (x,y)                     |     uv    |            color (rgba)
		rect.pos.x, rect.pos.y + rect.size.y,               0.0f, 1.0f, color.r, color.g, color.b, color.a,
		rect.pos.x, rect.pos.y,                             0.0f, 0.0f, color.r, color.g, color.b, color.a,
		rect.pos.x + rect.size.x, rect.pos.y,               1.0f, 0.0f, color.r, color.g, color.b, color.a,
		rect.pos.x + rect.size.x, rect.pos.y + rect.size.y, 1.0f, 1.0f, color.r, color.g, color.b, color.a,
	};

	uint32_t indices[] =
	{
		0, 1, 2,
		0, 2, 3,
	};

	LvnDrawCommand drawCmd{};
	drawCmd.pVertices = vertices;
	drawCmd.vertexAttributeCount = 8;
	drawCmd.vertexCount = 4;
	drawCmd.pIndices = indices;
	drawCmd.indexCount = 6;

	renderer->drawList.push_back(drawCmd);
}

void drawPoly(LvnRenderer* renderer, const LvnPoly& poly, const LvnVec4& color)
{
	float r = color.r;
	float g = color.g;
	float b = color.b;
	float a = color.a;

	std::vector<float> vertices(poly.nSides * 8);
	std::vector<uint32_t> indices(3 * (poly.nSides - 2));

	float doublePi = static_cast<float>(2 * LVN_PI);
	float angle = doublePi / poly.nSides;

	for (int i = 0; i < poly.nSides; i++)
	{
		float circlex = cos(i * angle);
		float circley = sin(i * angle);

		float posx = poly.pos.x + (poly.radius * circlex);
		float posy = poly.pos.y + (poly.radius * circley);

		vertices[i * 8]     = posx;
		vertices[i * 8 + 1] = posy;
		vertices[i * 8 + 2] = circlex;
		vertices[i * 8 + 3] = circlex;
		vertices[i * 8 + 4] = color.r;
		vertices[i * 8 + 5] = color.g;
		vertices[i * 8 + 6] = color.b;
		vertices[i * 8 + 7] = color.a;
	}

	for (int i = 0; i < poly.nSides - 2; i++)
	{
		indices[i * 3]     = 0;
		indices[i * 3 + 1] = i + 1;
		indices[i * 3 + 2] = i + 2;
	}

	LvnDrawCommand drawCmd{};
	drawCmd.pVertices = vertices.data();
	drawCmd.vertexAttributeCount = 8;
	drawCmd.vertexCount = poly.nSides;
	drawCmd.pIndices = indices.data();
	drawCmd.indexCount = indices.size();

	renderer->drawList.push_back(drawCmd);
}

} /* namespace lvn */
