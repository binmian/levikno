#include "levikno.h"
#include "levikno_internal.h"

#define LVN_ARRAY_LEN(x) (sizeof(x) / sizeof(x[0]))


static const char* s_Renderer2dDefaultVertexShaderCode = R"(
#version 460

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec4 inColor;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec4 fragColor;

layout(binding = 0) uniform ObjectBuffer
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

static const char* s_Renderer2dDefaultFragmentShaderCodeTypeVks = R"(
#version 460

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec4 fragColor;

layout(binding = 1) uniform sampler2D inTextures[];

void main()
{
	vec3 color = vec3(texture(inTextures[1], fragTexCoord) * fragColor);
	outColor = vec4(color, fragColor.a);
}
)";

static const char* s_Renderer2dDefaultFragmentShaderCodeTypeOgl = R"(
#version 460

#extension GL_ARB_bindless_texture : require

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec4 fragColor;

layout(binding = 1, std430) readonly buffer textureSsbo {
	sampler2D inTextures[];
};

void main()
{
	vec3 color = vec3(texture(inTextures[1], fragTexCoord) * fragColor);
	outColor = vec4(color, fragColor.a);
}
)";


static const char* s_RendererLineDefaultVertexShaderCode = R"(
#version 460

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec4 fragColor;

layout (binding = 0) uniform ObjectBuffer
{
	mat4 matrix;
} ubo;

void main()
{
	gl_Position = ubo.matrix * vec4(inPos, 1.0);
	fragColor = inColor;
}
)";

static const char* s_RendererLineDefaultFragmentShaderCode = R"(
#version 460

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec4 fragColor;

void main()
{
	outColor = fragColor;
}
)";

namespace lvn
{

static LvnResult initRenderer2d(LvnRenderer* renderer, LvnRendererModeData* modeData);
static LvnResult initRendererLine(LvnRenderer* renderer, LvnRendererModeData* modeData);
static void      drawRenderer2d(LvnRenderer* renderer, LvnRendererModeData* modeData);
static void      drawRendererLine(LvnRenderer* renderer, LvnRendererModeData* modeData);


static LvnResult initRenderer2d(LvnRenderer* renderer, LvnRendererModeData* modeData)
{
	modeData->maxVertexCount = 1024;
	modeData->maxIndexCount = 1024;
	modeData->maxTextures = 16;
	modeData->drawFunc = drawRenderer2d;
	modeData->mode = Lvn_RendererMode_2d;

	// create draw buffer
	LvnVertexAttribute attributes[] =
	{
		{ 0, 0, Lvn_VertexDataType_Vec2f, 0 },
		{ 0, 1, Lvn_VertexDataType_Vec2f, (2 * sizeof(float)) },
		{ 0, 2, Lvn_VertexDataType_Vec4f, (4 * sizeof(float)) },
	};

	LvnVertexBindingDescription vertexBindingDescription{};
	vertexBindingDescription.binding = 0;
	vertexBindingDescription.stride = 8 * sizeof(float);

	LvnBufferCreateInfo drawBufferCreateInfo{};
	drawBufferCreateInfo.type = Lvn_BufferType_DynamicVertex | Lvn_BufferType_DynamicIndex;
	drawBufferCreateInfo.pVertexAttributes = attributes;
	drawBufferCreateInfo.vertexAttributeCount = LVN_ARRAY_LEN(attributes);
	drawBufferCreateInfo.pVertexBindingDescriptions = &vertexBindingDescription;
	drawBufferCreateInfo.vertexBindingDescriptionCount = 1;
	drawBufferCreateInfo.pVertices = nullptr;
	drawBufferCreateInfo.vertexBufferSize = modeData->maxVertexCount * 8 * sizeof(float);
	drawBufferCreateInfo.pIndices = nullptr;
	drawBufferCreateInfo.indexBufferSize = modeData->maxIndexCount * sizeof(uint32_t);

	// create buffer
	if (lvn::createBuffer(&modeData->buffer, &drawBufferCreateInfo) != Lvn_Result_Success)
	{
		LVN_CORE_ERROR("cannot create 2d renderer, failed to create buffer");
		return Lvn_Result_Failure;
	}

	LvnShaderCreateInfo shaderCreateInfo{};
	shaderCreateInfo.vertexSrc = s_Renderer2dDefaultVertexShaderCode;

	switch (lvn::getGraphicsApi())
	{
		case Lvn_GraphicsApi_OpenGL:
		{
			shaderCreateInfo.fragmentSrc = s_Renderer2dDefaultFragmentShaderCodeTypeOgl;
			break;
		}
		case Lvn_GraphicsApi_Vulkan:
		{
			shaderCreateInfo.fragmentSrc = s_Renderer2dDefaultFragmentShaderCodeTypeVks;
			break;
		}

		default:
		{
			LVN_CORE_ERROR("cannot create 2d renderer, no graphics api selected");
			return Lvn_Result_Failure;
		}
	}

	LvnShader* shader;
	if (lvn::createShaderFromSrc(&shader, &shaderCreateInfo) != Lvn_Result_Success)
	{
		LVN_CORE_ERROR("cannot create 2d renderer, failed to create draw shaders");
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
	descriptorBindingTexture.descriptorType = Lvn_DescriptorType_ImageSamplerBindless;
	descriptorBindingTexture.shaderStage = Lvn_ShaderStage_Fragment;
	descriptorBindingTexture.descriptorCount = modeData->maxTextures;
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
	if (lvn::createDescriptorLayout(&modeData->descriptorLayout, &descriptorLayoutCreateInfo) != Lvn_Result_Success)
	{
		LVN_CORE_ERROR("cannot create 2d renderer, failed to create descriptor layout");
		return Lvn_Result_Failure;
	}
	if (lvn::createDescriptorSet(&modeData->descriptorSet, modeData->descriptorLayout) != Lvn_Result_Success)
	{
		LVN_CORE_ERROR("cannot create 2d renderer, failed to create descriptor set");
		return Lvn_Result_Failure;
	}


	LvnRenderPass* renderPass = lvn::windowGetRenderPass(renderer->window);
	LvnPipelineSpecification pipelineSpec = lvn::pipelineSpecificationGetConfig();

	// pipeline create info struct
	LvnPipelineCreateInfo pipelineCreateInfo{};
	pipelineCreateInfo.pipelineSpecification = &pipelineSpec;
	pipelineCreateInfo.pVertexAttributes = attributes;
	pipelineCreateInfo.vertexAttributeCount = LVN_ARRAY_LEN(attributes);
	pipelineCreateInfo.pVertexBindingDescriptions = &vertexBindingDescription;
	pipelineCreateInfo.vertexBindingDescriptionCount = 1;
	pipelineCreateInfo.pDescriptorLayouts = &modeData->descriptorLayout;
	pipelineCreateInfo.descriptorLayoutCount = 1;
	pipelineCreateInfo.shader = shader;
	pipelineCreateInfo.renderPass = renderPass;

	// create pipeline
	if (lvn::createPipeline(&modeData->pipeline, &pipelineCreateInfo) != Lvn_Result_Success)
	{
		LVN_CORE_ERROR("cannot create 2d renderer, failed to create pipeline");
		return Lvn_Result_Failure;
	}

	lvn::destroyShader(shader);


	// uniform buffer create info struct
	LvnUniformBufferCreateInfo uniformBufferCreateInfo{};
	uniformBufferCreateInfo.type = Lvn_BufferType_Uniform;
	uniformBufferCreateInfo.binding = 0;
	uniformBufferCreateInfo.size = sizeof(LvnRenderer::LvnRendererUniformData);

	// create uniform buffer
	lvn::createUniformBuffer(&modeData->uniformBuffer, &uniformBufferCreateInfo);


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
	textureCreateInfo.format = Lvn_TextureFormat_Unorm;
	textureCreateInfo.minFilter = Lvn_TextureFilter_Nearest;
	textureCreateInfo.magFilter = Lvn_TextureFilter_Nearest;
	textureCreateInfo.wrapS = Lvn_TextureMode_Repeat;
	textureCreateInfo.wrapT = Lvn_TextureMode_Repeat;

	lvn::createTexture(&renderer->texture, &textureCreateInfo);

	// update descriptor set
	LvnUniformBufferInfo bufferInfo{};
	bufferInfo.buffer = modeData->uniformBuffer;
	bufferInfo.range = sizeof(LvnRenderer::LvnRendererUniformData);
	bufferInfo.offset = 0;

	LvnDescriptorUpdateInfo descriptorUniformUpdateInfo{};
	descriptorUniformUpdateInfo.descriptorType = Lvn_DescriptorType_UniformBuffer;
	descriptorUniformUpdateInfo.binding = 0;
	descriptorUniformUpdateInfo.descriptorCount = 1;
	descriptorUniformUpdateInfo.bufferInfo = &bufferInfo;

	std::vector<LvnTexture*> defaultTextures(modeData->maxTextures, renderer->texture);

	LvnDescriptorUpdateInfo descriptorTextureUpdateInfo{};
	descriptorTextureUpdateInfo.descriptorType = Lvn_DescriptorType_ImageSamplerBindless;
	descriptorTextureUpdateInfo.binding = 1;
	descriptorTextureUpdateInfo.descriptorCount = defaultTextures.size();
	descriptorTextureUpdateInfo.pTextureInfos = defaultTextures.data();

	LvnDescriptorUpdateInfo descriptorUpdateInfos[] =
	{
		descriptorUniformUpdateInfo, descriptorTextureUpdateInfo,
	};

	lvn::updateDescriptorSetData(modeData->descriptorSet, descriptorUpdateInfos, LVN_ARRAY_LEN(descriptorUpdateInfos));

	return Lvn_Result_Success;
}

static LvnResult initRendererLine(LvnRenderer* renderer, LvnRendererModeData* modeData)
{
	modeData->maxVertexCount = 1024;
	modeData->maxIndexCount = 1024;
	modeData->maxTextures = 0;
	modeData->drawFunc = drawRendererLine;
	modeData->mode = Lvn_RendererMode_Line;

	// create draw buffer
	LvnVertexAttribute attributes[] =
	{
		{ 0, 0, Lvn_VertexDataType_Vec3f, 0 },
		{ 0, 1, Lvn_VertexDataType_Vec4f, (3 * sizeof(float)) },
	};

	LvnVertexBindingDescription vertexBindingDescription{};
	vertexBindingDescription.binding = 0;
	vertexBindingDescription.stride = 7 * sizeof(float);

	LvnBufferCreateInfo drawBufferCreateInfo{};
	drawBufferCreateInfo.type = Lvn_BufferType_DynamicVertex | Lvn_BufferType_DynamicIndex;
	drawBufferCreateInfo.pVertexAttributes = attributes;
	drawBufferCreateInfo.vertexAttributeCount = LVN_ARRAY_LEN(attributes);
	drawBufferCreateInfo.pVertexBindingDescriptions = &vertexBindingDescription;
	drawBufferCreateInfo.vertexBindingDescriptionCount = 1;
	drawBufferCreateInfo.pVertices = nullptr;
	drawBufferCreateInfo.vertexBufferSize = modeData->maxVertexCount * 7 * sizeof(float);
	drawBufferCreateInfo.pIndices = nullptr;
	drawBufferCreateInfo.indexBufferSize = modeData->maxIndexCount * sizeof(uint32_t);

	// create buffer
	if (lvn::createBuffer(&modeData->buffer, &drawBufferCreateInfo) != Lvn_Result_Success)
	{
		LVN_CORE_ERROR("cannot create line renderer, failed to create buffer");
		return Lvn_Result_Failure;
	}

	LvnShaderCreateInfo shaderCreateInfo{};
	shaderCreateInfo.vertexSrc = s_RendererLineDefaultVertexShaderCode;
	shaderCreateInfo.fragmentSrc = s_RendererLineDefaultFragmentShaderCode;

	LvnShader* shader;
	if (lvn::createShaderFromSrc(&shader, &shaderCreateInfo) != Lvn_Result_Success)
	{
		LVN_CORE_ERROR("cannot create line renderer, failed to create draw shaders");
		return Lvn_Result_Failure;
	}


	// descriptor binding
	LvnDescriptorBinding descriptorBindingUniform{};
	descriptorBindingUniform.binding = 0;
	descriptorBindingUniform.descriptorType = Lvn_DescriptorType_UniformBuffer;
	descriptorBindingUniform.shaderStage = Lvn_ShaderStage_Vertex;
	descriptorBindingUniform.descriptorCount = 1;
	descriptorBindingUniform.maxAllocations = 1;

	// descriptor layout create info
	LvnDescriptorLayoutCreateInfo descriptorLayoutCreateInfo{};
	descriptorLayoutCreateInfo.pDescriptorBindings = &descriptorBindingUniform;
	descriptorLayoutCreateInfo.descriptorBindingCount = 1;
	descriptorLayoutCreateInfo.maxSets = 1;

	// create descriptor layout and set
	if (lvn::createDescriptorLayout(&modeData->descriptorLayout, &descriptorLayoutCreateInfo) != Lvn_Result_Success)
	{
		LVN_CORE_ERROR("cannot create line renderer, failed to create descriptor layout");
		return Lvn_Result_Failure;
	}
	if (lvn::createDescriptorSet(&modeData->descriptorSet, modeData->descriptorLayout) != Lvn_Result_Success)
	{
		LVN_CORE_ERROR("cannot create line renderer, failed to create descriptor set");
		return Lvn_Result_Failure;
	}


	LvnRenderPass* renderPass = lvn::windowGetRenderPass(renderer->window);
	LvnPipelineSpecification pipelineSpec = lvn::pipelineSpecificationGetConfig();
	pipelineSpec.inputAssembly.topology = Lvn_TopologyType_Line;

	// pipeline create info struct
	LvnPipelineCreateInfo pipelineCreateInfo{};
	pipelineCreateInfo.pipelineSpecification = &pipelineSpec;
	pipelineCreateInfo.pVertexAttributes = attributes;
	pipelineCreateInfo.vertexAttributeCount = LVN_ARRAY_LEN(attributes);
	pipelineCreateInfo.pVertexBindingDescriptions = &vertexBindingDescription;
	pipelineCreateInfo.vertexBindingDescriptionCount = 1;
	pipelineCreateInfo.pDescriptorLayouts = &modeData->descriptorLayout;
	pipelineCreateInfo.descriptorLayoutCount = 1;
	pipelineCreateInfo.shader = shader;
	pipelineCreateInfo.renderPass = renderPass;

	// create pipeline
	if (lvn::createPipeline(&modeData->pipeline, &pipelineCreateInfo) != Lvn_Result_Success)
	{
		LVN_CORE_ERROR("cannot create line renderer, failed to create pipeline");
		return Lvn_Result_Failure;
	}

	lvn::destroyShader(shader);


	// uniform buffer create info struct
	LvnUniformBufferCreateInfo uniformBufferCreateInfo{};
	uniformBufferCreateInfo.type = Lvn_BufferType_Uniform;
	uniformBufferCreateInfo.binding = 0;
	uniformBufferCreateInfo.size = sizeof(LvnRenderer::LvnRendererUniformData);

	// create uniform buffer
	lvn::createUniformBuffer(&modeData->uniformBuffer, &uniformBufferCreateInfo);


	// update descriptor set
	LvnUniformBufferInfo bufferInfo{};
	bufferInfo.buffer = modeData->uniformBuffer;
	bufferInfo.range = sizeof(LvnRenderer::LvnRendererUniformData);
	bufferInfo.offset = 0;

	LvnDescriptorUpdateInfo descriptorUniformUpdateInfo{};
	descriptorUniformUpdateInfo.descriptorType = Lvn_DescriptorType_UniformBuffer;
	descriptorUniformUpdateInfo.binding = 0;
	descriptorUniformUpdateInfo.descriptorCount = 1;
	descriptorUniformUpdateInfo.bufferInfo = &bufferInfo;

	lvn::updateDescriptorSetData(modeData->descriptorSet, &descriptorUniformUpdateInfo, 1);

	return Lvn_Result_Success;
}

static void drawRenderer2d(LvnRenderer* renderer, LvnRendererModeData* modeData)
{
	int width, height;
	lvn::windowGetSize(renderer->window, &width, &height);

	LvnMat4 proj = lvn::ortho((float)width * -0.5f, (float)width * 0.5f, (float)height * -0.5f, (float)height * 0.5f, -1.0f, 1.0f);
	LvnMat4 view = LvnMat4(1.0f);
	LvnMat4 camera = proj * view;

	if (modeData->drawList.vertex_count() > modeData->maxVertexCount)
	{
		lvn::bufferResizeVertexBuffer(modeData->buffer, static_cast<uint64_t>(modeData->drawList.vertex_count() * sizeof(float) * 1.5));
		modeData->maxVertexCount = modeData->drawList.vertex_count();
	}
	if (modeData->drawList.index_count() > modeData->maxIndexCount)
	{
		lvn::bufferResizeIndexBuffer(modeData->buffer, static_cast<uint64_t>(modeData->drawList.index_count() * sizeof(uint32_t) * 1.5));
		modeData->maxIndexCount = modeData->drawList.index_count();
	}

	renderer->uniformData2d.matrix = camera;
	lvn::updateUniformBufferData(renderer->window, modeData->uniformBuffer, &renderer->uniformData2d, sizeof(renderer->uniformData2d), 0);

	lvn::bufferUpdateVertexData(modeData->buffer, modeData->drawList.vertices(), modeData->drawList.vertex_size(), 0);
	lvn::bufferUpdateIndexData(modeData->buffer, modeData->drawList.indices(), modeData->drawList.index_size(), 0);

	lvn::renderCmdBindPipeline(renderer->window, modeData->pipeline);
	lvn::renderCmdBindDescriptorSets(renderer->window, modeData->pipeline, 0, 1, &modeData->descriptorSet);

	lvn::renderCmdBindVertexBuffer(renderer->window, modeData->buffer);
	lvn::renderCmdBindIndexBuffer(renderer->window, modeData->buffer);

	lvn::renderCmdDrawIndexed(renderer->window, modeData->drawList.index_count());
}

static void drawRendererLine(LvnRenderer* renderer, LvnRendererModeData* modeData)
{
	int width, height;
	lvn::windowGetSize(renderer->window, &width, &height);

	LvnMat4 proj = lvn::ortho((float)width * -0.5f, (float)width * 0.5f, (float)height * -0.5f, (float)height * 0.5f, -1.0f, 1.0f);
	LvnMat4 view = LvnMat4(1.0f);
	LvnMat4 camera = proj * view;

	if (modeData->drawList.vertex_count() > modeData->maxVertexCount)
	{
		lvn::bufferResizeVertexBuffer(modeData->buffer, static_cast<uint64_t>(modeData->drawList.vertex_count() * sizeof(float) * 1.5));
		modeData->maxVertexCount = modeData->drawList.vertex_count();
	}
	if (modeData->drawList.index_count() > modeData->maxIndexCount)
	{
		lvn::bufferResizeIndexBuffer(modeData->buffer, static_cast<uint64_t>(modeData->drawList.index_count() * sizeof(uint32_t) * 1.5));
		modeData->maxIndexCount = modeData->drawList.index_count();
	}

	renderer->uniformData2d.matrix = camera;
	lvn::updateUniformBufferData(renderer->window, modeData->uniformBuffer, &renderer->uniformData2d, sizeof(renderer->uniformData2d), 0);

	lvn::bufferUpdateVertexData(modeData->buffer, modeData->drawList.vertices(), modeData->drawList.vertex_size(), 0);
	lvn::bufferUpdateIndexData(modeData->buffer, modeData->drawList.indices(), modeData->drawList.index_size(), 0);

	lvn::renderCmdBindPipeline(renderer->window, modeData->pipeline);
	lvn::renderCmdBindDescriptorSets(renderer->window, modeData->pipeline, 0, 1, &modeData->descriptorSet);

	lvn::renderCmdBindVertexBuffer(renderer->window, modeData->buffer);
	lvn::renderCmdBindIndexBuffer(renderer->window, modeData->buffer);

	lvn::renderCmdDrawIndexed(renderer->window, modeData->drawList.index_count());
}

LvnResult rendererInit(LvnRenderer* renderer, LvnRendererCreateInfo* createInfo)
{
	if (!renderer)
	{
		LVN_CORE_ERROR("cannot create renderer, parameter renderer was nullptr");
		return Lvn_Result_Failure;
	}

	if (createInfo->rendererModes == Lvn_RendererMode_None)
	{
		LVN_CORE_WARN("renderer (%p) has no renderer mode set on initiation", renderer);
	}

	*renderer = {};

	if (lvn::createWindow(&renderer->window, &createInfo->windowCreateInfo) != Lvn_Result_Success)
	{
		LVN_CORE_ERROR("cannot create renderer, failed to create window");
		return Lvn_Result_Failure;
	}

	renderer->backGroundColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	renderer->renderModes = createInfo->rendererModes;

	LvnRendererModeData modeData2d{};

	if (createInfo->rendererModes & Lvn_RendererMode_2d)
	{
		if (lvn::initRenderer2d(renderer, &modeData2d) != Lvn_Result_Success)
		{
			LVN_CORE_ERROR("failed to create 2d renderer (%p)", renderer);
			return Lvn_Result_Failure;
		}

		renderer->rendererModes.push_back(modeData2d);
	}

	LvnRendererModeData modeDataLine{};

	if (createInfo->rendererModes & Lvn_RendererMode_Line)
	{
		if (lvn::initRendererLine(renderer, &modeDataLine) != Lvn_Result_Success)
		{
			LVN_CORE_ERROR("failed to create line renderer (%p)", renderer);
			return Lvn_Result_Failure;
		}

		renderer->rendererModes.push_back(modeDataLine);
	}

	for (auto& modeData : renderer->rendererModes)
	{
		renderer->rendererModesIndices[modeData.mode] = &modeData;
	}

	return Lvn_Result_Success;
}

void rendererTerminate(LvnRenderer* renderer)
{
	for (auto& modeData : renderer->rendererModes)
	{
		lvn::destroyBuffer(modeData.buffer);
		lvn::destroyPipeline(modeData.pipeline);
		lvn::destroyDescriptorLayout(modeData.descriptorLayout);
		lvn::destroyDescriptorSet(modeData.descriptorSet);
		lvn::destroyUniformBuffer(modeData.uniformBuffer);
	}

	if (renderer->window) { lvn::destroyWindow(renderer->window); }
	if (renderer->texture) { lvn::destroyTexture(renderer->texture); }
}

void rendererBeginDraw(LvnRenderer* renderer)
{
	if (renderer->currentSpriteCount != renderer->spriteTextures.size())
	{
		renderer->currentSpriteCount = renderer->spriteTextures.size();

		LvnRendererModeData* modeData = renderer->rendererModesIndices[Lvn_RendererMode_2d];

		std::vector<LvnTexture*> descriptorTextures = { renderer->texture };
		descriptorTextures.insert(descriptorTextures.end(), renderer->spriteTextures.begin(), renderer->spriteTextures.end());

		LvnDescriptorUpdateInfo descriptorTextureUpdateInfo{};
		descriptorTextureUpdateInfo.descriptorType = Lvn_DescriptorType_ImageSamplerBindless;
		descriptorTextureUpdateInfo.binding = 1;
		descriptorTextureUpdateInfo.descriptorCount = descriptorTextures.size();
		descriptorTextureUpdateInfo.pTextureInfos = descriptorTextures.data();

		lvn::updateDescriptorSetData(modeData->descriptorSet, &descriptorTextureUpdateInfo, 1);
	}

	lvn::windowUpdate(renderer->window);

	lvn::renderBeginNextFrame(renderer->window);
	lvn::renderBeginCommandRecording(renderer->window);

	lvn::renderClearColor(renderer->window, renderer->backGroundColor.r, renderer->backGroundColor.g, renderer->backGroundColor.b, renderer->backGroundColor.a);
	lvn::renderCmdBeginRenderPass(renderer->window);
}

void rendererEndDraw(LvnRenderer* renderer)
{
	for (auto& modeData : renderer->rendererModes)
	{
		modeData.drawFunc(renderer, &modeData);
	}

	lvn::renderCmdEndRenderPass(renderer->window);
	lvn::renderEndCommandRecording(renderer->window);
	lvn::renderDrawSubmit(renderer->window);

	for (auto& modeData : renderer->rendererModes)
	{
		modeData.drawList.clear();
	}
}

void rendererFlushDraw(LvnRenderer* renderer)
{
	for (auto& modeData : renderer->rendererModes)
	{
		modeData.drawFunc(renderer, &modeData);
		modeData.drawList.clear();
	}
}

void rendererFlushDrawMode(LvnRenderer* renderer, LvnRendererMode renderMode)
{
	LvnRendererModeData* modeData = renderer->rendererModesIndices[renderMode];
	modeData->drawFunc(renderer, modeData);

	modeData->drawList.clear();
}

void rendererSetBackgroundColor(LvnRenderer* renderer, float r, float g, float b, float a)
{
	renderer->backGroundColor = { r, g, b, a };
}

void rendererAddSprite(LvnRenderer* renderer, LvnSprite* sprite)
{
	if (!renderer->availableSpriteIndices.empty())
	{
		uint32_t index = renderer->availableSpriteIndices.front();
		renderer->availableSpriteIndices.pop();

		renderer->spriteTextures[index] = sprite->texture;
		renderer->spriteTexturesIndices[sprite->id] = sprite->texture - renderer->spriteTextures.front();
		return;
	}

	renderer->spriteTextures.push_back(sprite->texture);
	renderer->spriteTexturesIndices[sprite->id] = renderer->spriteTextures.size() - 1;
}

void rendererRemoveSprite(LvnRenderer* renderer, LvnSprite* sprite)
{
	LvnTexture* texture = renderer->spriteTextures[renderer->spriteTexturesIndices[sprite->id]];
	renderer->availableSpriteIndices.push(texture - renderer->spriteTextures.front());
}

void drawTriangle(LvnRenderer* renderer, const LvnTriangle& triangle, const LvnColor& color)
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

	renderer->rendererModesIndices[Lvn_RendererMode_2d]->drawList.push_back(drawCmd);
}

void drawRect(LvnRenderer* renderer, const LvnRect& rect, const LvnColor& color)
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

	renderer->rendererModesIndices[Lvn_RendererMode_2d]->drawList.push_back(drawCmd);
}

void drawPoly(LvnRenderer* renderer, const LvnPoly& poly, const LvnColor& color)
{
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
		vertices[i * 8 + 3] = circley;
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

	renderer->rendererModesIndices[Lvn_RendererMode_2d]->drawList.push_back(drawCmd);
}

void drawLine(LvnRenderer* renderer, const LvnLine& line, const LvnColor& color)
{
	float vertices[] =
	{
		line.v1.x, line.v1.y, line.v1.z, color.r, color.g, color.b, color.a,
		line.v2.x, line.v2.y, line.v2.z, color.r, color.g, color.b, color.a,
	};

	uint32_t indices[] =
	{
		0, 1,
	};

	LvnDrawCommand drawCmd{};
	drawCmd.pVertices = vertices;
	drawCmd.vertexAttributeCount = 7;
	drawCmd.vertexCount = 2;
	drawCmd.pIndices = indices;
	drawCmd.indexCount = 2;

	renderer->rendererModesIndices[Lvn_RendererMode_Line]->drawList.push_back(drawCmd);
}

void drawSprite(LvnRenderer* renderer, LvnSprite& sprite, const LvnColor& color)
{
	drawSpriteEx(renderer, sprite, color, 1.0f, 0.0f);
}

void drawSpriteEx(LvnRenderer* renderer, LvnSprite& sprite, const LvnColor& color, float scale, float rotation)
{

	float width = static_cast<float>(sprite.texture->width);
	float height = static_cast<float>(sprite.texture->height);

	LvnVec2 bl = sprite.pos;
	LvnVec2 br = sprite.pos + lvn::rotate(LvnVec2(width * scale, 0.0f), rotation);
	LvnVec2 tl = sprite.pos + lvn::rotate(LvnVec2(0.0f, height * scale), rotation);
	LvnVec2 tr = sprite.pos + lvn::rotate(LvnVec2(width * scale, height * scale), rotation);

	float vertices[] =
	{
	//   pos (x,y) |     uv    |          color (rgba)
		tl.x, tl.y, 0.0f, 1.0f, color.r, color.g, color.b, color.a, // TL
		bl.x, bl.y, 0.0f, 0.0f, color.r, color.g, color.b, color.a, // BL (origin)
		br.x, br.y, 1.0f, 0.0f, color.r, color.g, color.b, color.a, // BR
		tr.x, tr.y, 1.0f, 1.0f, color.r, color.g, color.b, color.a, // TR
	};

	uint32_t indices[] =
	{
		0, 1, 2,
		0, 2, 3,
	};

	LvnRendererModeData* modeData = renderer->rendererModesIndices[Lvn_RendererMode_2d];

	LvnDrawCommand drawCmd{};
	drawCmd.pVertices = vertices;
	drawCmd.vertexAttributeCount = 8;
	drawCmd.vertexCount = 4;
	drawCmd.pIndices = indices;
	drawCmd.indexCount = 6;

	renderer->rendererModesIndices[Lvn_RendererMode_2d]->drawList.push_back(drawCmd);
}

LvnSprite createSprite(LvnTextureCreateInfo* createInfo, const LvnVec2& pos)
{
	LvnContext* lvnctx = lvn::getContext();

	LvnTexture* texture;
	lvn::createTexture(&texture, createInfo);

	LvnSprite sprite =
	{
		texture,
		pos,
		lvnctx->spriteIDs.take_next()
	};

	return sprite;
}

void destroySprite(LvnSprite* sprite)
{
	LvnContext* lvnctx = lvn::getContext();
	if (!lvnctx->spriteIDs.push_back(sprite->id))
		LVN_CORE_ASSERT(false, "sprite does not have a valid id that is being used");

	lvn::destroyTexture(sprite->texture);
}

} /* namespace lvn */
