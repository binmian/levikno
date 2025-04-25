#include "levikno.h"
#include "levikno_internal.h"


#define LVN_ARRAY_LEN(x) (sizeof(x) / sizeof(x[0]))

static const char* s_VertexShaderSrc = R"(
#version 460

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

layout (binding = 0) uniform ObjectBuffer
{
	mat4 u_ProjMat;
	mat4 u_ViewMat;
};

void main()
{
	gl_Position = u_ProjMat * u_ViewMat * vec4(inPos, 0.0, 1.0);
	fragColor = inColor;
	fragTexCoord = inTexCoord;
}
)";

static const char* s_FragmentShaderSrc = R"(
#version 460

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(binding = 1) uniform sampler2D inTexture;

void main()
{
	outColor = vec4(fragColor * vec3(texture(inTexture, fragTexCoord)), 1.0);
}
)";

struct LvnUniformData
{
	LvnMat4 projMat;
	LvnMat4 viewMat;
};

struct LvnVertexData2d
{
	LvnVec2 pos;
	LvnColor color;
	LvnVec2 texCoords;
};

namespace lvn
{

static LvnResult     createRendererResources(const LvnWindowCreateInfo* windowCreateInfo);
static LvnRenderMode createRenderMode(const LvnRenderer* renderer, LvnRenderModeEnum renderModeEnum);
static void          renderModeDraw2d(LvnRenderer* renderer, LvnRenderMode& renderMode);

static LvnResult createRendererResources(const LvnWindowCreateInfo* windowCreateInfo)
{
	LvnContext* lvnctx = lvn::getContext();

	if (lvnctx->renderer)
		return Lvn_Result_AlreadyCalled;

	lvnctx->renderer = std::make_unique<LvnRenderer>();
	LvnRenderer* renderer = lvnctx->renderer.get();

	if (lvn::createWindow(&renderer->window, windowCreateInfo) != Lvn_Result_Success)
		return Lvn_Result_Failure;

	// set background clear color
	renderer->clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };

	// texture
	uint8_t whiteTextureData[] = { 0xff, 0xff, 0xff, 0xff };
	LvnImageData imageData;
	imageData.pixels = LvnData<uint8_t>(whiteTextureData, sizeof(whiteTextureData) / sizeof(uint8_t));
	imageData.width = 1;
	imageData.height = 1;
	imageData.channels = 4;
	imageData.size = 4;

	LvnTextureCreateInfo textureCreateInfo{};
	textureCreateInfo.imageData = imageData;
	textureCreateInfo.format = Lvn_TextureFormat_Unorm;
	textureCreateInfo.wrapS = Lvn_TextureMode_Repeat;
	textureCreateInfo.wrapT = Lvn_TextureMode_Repeat;
	textureCreateInfo.minFilter = Lvn_TextureFilter_Linear;
	textureCreateInfo.magFilter = Lvn_TextureFilter_Linear;

	// create texture
	lvn::createTexture(&renderer->defaultWhiteTexture, &textureCreateInfo);


	// render modes
	renderer->renderModes.resize(Lvn_RenderMode_Max_Value);

	renderer->renderModes[Lvn_RenderMode_2d] = std::move(lvn::createRenderMode(renderer, Lvn_RenderMode_2d));


	return Lvn_Result_Success;
}

static LvnRenderMode createRenderMode(const LvnRenderer* renderer, LvnRenderModeEnum renderModeEnum)
{
	LvnRenderMode renderMode{};
	renderMode.maxVertexCount = 5000;
	renderMode.maxIndexCount = 5000;

	uint32_t stride = sizeof(LvnVertexData2d);

	// create buffer
	LvnBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.type = Lvn_BufferType_Vertex | Lvn_BufferType_Index;
	bufferCreateInfo.usage = Lvn_BufferUsage_Dynamic;
	bufferCreateInfo.data = nullptr;
	bufferCreateInfo.size = renderMode.maxVertexCount * stride + renderMode.maxIndexCount * sizeof(uint32_t);

	lvn::createBuffer(&renderMode.buffer, &bufferCreateInfo);
	renderMode.indexOffset = renderMode.maxVertexCount * stride;

	// attributes and bindings

	renderMode.bindingDescriptions = {LvnVertexBindingDescription{ 0, stride }};
	renderMode.attributes =
	{
		{ 0, Lvn_AttributeLocation_Position, Lvn_AttributeFormat_Vec2_f32, 0 },
		{ 0, Lvn_AttributeLocation_Color, Lvn_AttributeFormat_Vec3_un8, (2 * sizeof(float)) },
		{ 0, Lvn_AttributeLocation_TexCoords, Lvn_AttributeFormat_Vec2_f32, (2 * sizeof(float) + 4 * sizeof(uint8_t)) },
	};

	// create pipeline
	LvnShaderCreateInfo shaderCreateInfo{};
	shaderCreateInfo.vertexSrc = s_VertexShaderSrc;
	shaderCreateInfo.fragmentSrc = s_FragmentShaderSrc;

	LvnShader* shader;
	lvn::createShaderFromSrc(&shader, &shaderCreateInfo);

	// descriptor binding
	LvnDescriptorBinding descriptorUniformBinding{};
	descriptorUniformBinding.binding = 0;
	descriptorUniformBinding.descriptorType = Lvn_DescriptorType_UniformBuffer;
	descriptorUniformBinding.shaderStage = Lvn_ShaderStage_Vertex;
	descriptorUniformBinding.descriptorCount = 1;
	descriptorUniformBinding.maxAllocations = 1;

	LvnDescriptorBinding descriptorTextureBinding{};
	descriptorTextureBinding.binding = 1;
	descriptorTextureBinding.descriptorType = Lvn_DescriptorType_ImageSampler;
	descriptorTextureBinding.shaderStage = Lvn_ShaderStage_Fragment;
	descriptorTextureBinding.descriptorCount = 1;
	descriptorTextureBinding.maxAllocations = 1;

	LvnDescriptorBinding descriptorBindings[] =
	{
		descriptorUniformBinding, descriptorTextureBinding,
	};

	// descriptor layout
	LvnDescriptorLayoutCreateInfo descriptorLayoutCreateInfo{};
	descriptorLayoutCreateInfo.pDescriptorBindings = descriptorBindings;
	descriptorLayoutCreateInfo.descriptorBindingCount = LVN_ARRAY_LEN(descriptorBindings);
	descriptorLayoutCreateInfo.maxSets = 1;

	lvn::createDescriptorLayout(&renderMode.descriptorLayout, &descriptorLayoutCreateInfo);
	lvn::allocateDescriptorSet(&renderMode.descriptorSet, renderMode.descriptorLayout);

	LvnRenderPass* renderPass = lvn::windowGetRenderPass(renderer->window);
	LvnPipelineSpecification pipelineSpec = lvn::configPipelineSpecificationInit();

	// pipeline create info struct
	LvnPipelineCreateInfo pipelineCreateInfo{};
	pipelineCreateInfo.pipelineSpecification = &pipelineSpec;
	pipelineCreateInfo.pVertexAttributes = renderMode.attributes.data();
	pipelineCreateInfo.vertexAttributeCount = renderMode.attributes.size();
	pipelineCreateInfo.pVertexBindingDescriptions = renderMode.bindingDescriptions.data();
	pipelineCreateInfo.vertexBindingDescriptionCount = renderMode.bindingDescriptions.size();
	pipelineCreateInfo.pDescriptorLayouts = &renderMode.descriptorLayout;
	pipelineCreateInfo.descriptorLayoutCount = 1;
	pipelineCreateInfo.shader = shader;
	pipelineCreateInfo.renderPass = renderPass;

	// create pipeline
	lvn::createPipeline(&renderMode.pipeline, &pipelineCreateInfo);
	lvn::destroyShader(shader);


	// uniform buffer
	LvnUniformBufferCreateInfo uniformBufferCreateInfo{};
	uniformBufferCreateInfo.type = Lvn_BufferType_Uniform;
	uniformBufferCreateInfo.size = sizeof(LvnUniformData);

	// create uniform buffer
	lvn::createUniformBuffer(&renderMode.uniformBuffer, &uniformBufferCreateInfo);


	// update descriptor set
	LvnUniformBufferInfo bufferInfo{};
	bufferInfo.buffer = renderMode.uniformBuffer;
	bufferInfo.range = sizeof(LvnUniformData);
	bufferInfo.offset = 0;

	LvnDescriptorUpdateInfo descriptorUniformUpdateInfo{};
	descriptorUniformUpdateInfo.descriptorType = Lvn_DescriptorType_UniformBuffer;
	descriptorUniformUpdateInfo.binding = 0;
	descriptorUniformUpdateInfo.descriptorCount = 1;
	descriptorUniformUpdateInfo.bufferInfo = &bufferInfo;

	LvnDescriptorUpdateInfo descriptorTextureUpdateInfo{};
	descriptorTextureUpdateInfo.descriptorType = Lvn_DescriptorType_ImageSampler;
	descriptorTextureUpdateInfo.binding = 1;
	descriptorTextureUpdateInfo.descriptorCount = 1;
	descriptorTextureUpdateInfo.pTextureInfos = &renderer->defaultWhiteTexture;

	LvnDescriptorUpdateInfo descriptorUpdateInfos[] =
	{
		descriptorUniformUpdateInfo, descriptorTextureUpdateInfo,
	};

	lvn::updateDescriptorSetData(renderMode.descriptorSet, descriptorUpdateInfos, LVN_ARRAY_LEN(descriptorUpdateInfos));

	renderMode.drawFunc = lvn::renderModeDraw2d;

	return renderMode;
}

static void renderModeDraw2d(LvnRenderer* renderer, LvnRenderMode& renderMode)
{
	if (renderMode.drawList.empty())
		return;

	int width, height;
	lvn::windowGetSize(renderer->window, &width, &height);

	LvnUniformData uniformData{};
	uniformData.projMat = lvn::ortho((float)width * -0.5f, (float)width * 0.5f, (float)height * -0.5f, (float)height * 0.5f, -1.0f, 1.0f);
	uniformData.viewMat = LvnMat4(1.0f);

	lvn::updateUniformBufferData(renderMode.uniformBuffer, &uniformData, sizeof(LvnUniformData), 0);

	lvn::bufferUpdateData(renderMode.buffer, renderMode.drawList.vertices(), renderMode.drawList.vertex_size(), 0);
	lvn::bufferUpdateData(renderMode.buffer, renderMode.drawList.indices(), renderMode.drawList.index_size(), renderMode.indexOffset);

	lvn::renderCmdBindPipeline(renderer->window, renderMode.pipeline);
	lvn::renderCmdBindDescriptorSets(renderer->window, renderMode.pipeline, 0, 1, &renderMode.descriptorSet);

	lvn::renderCmdBindVertexBuffer(renderer->window, 0, 1, &renderMode.buffer, 0);
	lvn::renderCmdBindIndexBuffer(renderer->window, renderMode.buffer, renderMode.indexOffset);

	lvn::renderCmdDrawIndexed(renderer->window, renderMode.drawList.index_count());
}


LvnResult renderInit(const char* title, int width, int height)
{
	LvnWindowCreateInfo windowCreateInfo = lvn::configWindowInit(title, width, height);
	return createRendererResources(&windowCreateInfo);
}

LvnResult renderInit(const LvnWindowCreateInfo* createInfo)
{
	return createRendererResources(createInfo);
}

void renderTerminate()
{
	LvnContext* lvnctx = lvn::getContext();

	if (!lvnctx->renderer)
		return;

	LvnRenderer* renderer = lvnctx->renderer.get();

	for (auto& renderMode : renderer->renderModes)
	{
		lvn::destroyPipeline(renderMode.pipeline);
		lvn::destroyDescriptorLayout(renderMode.descriptorLayout);
		lvn::destroyBuffer(renderMode.buffer);
		lvn::destroyUniformBuffer(renderMode.uniformBuffer);
	}

	lvn::destroyTexture(renderer->defaultWhiteTexture);
	lvn::destroyWindow(renderer->window);

	lvnctx->renderer.reset(nullptr);
}

LvnWindow* getRendererWindow()
{
	LvnRenderer* renderer = lvn::getContext()->renderer.get();
	return renderer->window;
}

bool renderWindowOpen()
{
	LvnRenderer* renderer = lvn::getContext()->renderer.get();
	return lvn::windowOpen(renderer->window);
}

void drawBegin()
{
	LvnRenderer* renderer = lvn::getContext()->renderer.get();
	lvn::windowUpdate(renderer->window);

	for (auto& renderMode : renderer->renderModes)
		renderMode.drawList.clear();

	lvn::renderBeginNextFrame(renderer->window);
	lvn::renderBeginCommandRecording(renderer->window);
	lvn::renderCmdBeginRenderPass(renderer->window, renderer->clearColor.r, renderer->clearColor.g, renderer->clearColor.b, renderer->clearColor.a);
}

void drawEnd()
{
	LvnRenderer* renderer = lvn::getContext()->renderer.get();

	for (auto& renderMode : renderer->renderModes)
	{
		renderMode.drawFunc(renderer, renderMode);
	}

	lvn::renderCmdEndRenderPass(renderer->window);
	lvn::renderEndCommandRecording(renderer->window);
	lvn::renderDrawSubmit(renderer->window);
}

void drawClearColor(float r, float g, float b, float a)
{
	LvnRenderer* renderer = lvn::getContext()->renderer.get();
	renderer->clearColor = { r, g, b, a };
}

void drawClearColor(const LvnColor& color)
{
	LvnRenderer* renderer = lvn::getContext()->renderer.get();
	renderer->clearColor = { (float)color.r/255.0f, (float)color.g/255.0f, (float)color.b/255.0f, (float)color.a/255.0f };
}

void drawTriangle(const LvnVec2& v1, const LvnVec2& v2, const LvnVec2& v3, const LvnColor& color)
{
	LvnVertexData2d vertices[] =
	{
		{ v1, color, {0.0f, 0.0f} },
		{ v2, color, {0.5f, 1.0f} },
		{ v3, color, {1.0f, 0.0f} },
	};

	uint32_t indices[] = { 0, 1, 2 };

	LvnDrawCommand drawCmd{};
	drawCmd.pVertices = vertices;
	drawCmd.vertexCount = 3;
	drawCmd.pIndices = indices;
	drawCmd.indexCount = 3;
	drawCmd.vertexStride = sizeof(LvnVertexData2d);

	LvnRenderer* renderer = lvn::getContext()->renderer.get();
	renderer->renderModes[Lvn_RenderMode_2d].drawList.push_back(drawCmd);
}

void drawRect(const LvnVec2& pos, const LvnVec2& size, const LvnColor& color)
{
	LvnVertexData2d vertices[] =
	{
		{{ pos.x, pos.y + size.y },          color, {0.0f, 1.0f} },
		{{ pos.x, pos.y },                   color, {0.0f, 0.0f} },
		{{ pos.x + size.x, pos.y },          color, {1.0f, 0.0f} },
		{{ pos.x + size.x, pos.y + size.y }, color, {1.0f, 1.0f} },
	};

	uint32_t indices[] = { 0, 1, 2, 0, 2, 3 };

	LvnDrawCommand drawCmd{};
	drawCmd.pVertices = vertices;
	drawCmd.vertexCount = 4;
	drawCmd.pIndices = indices;
	drawCmd.indexCount = 6;
	drawCmd.vertexStride = sizeof(LvnVertexData2d);

	LvnRenderer* renderer = lvn::getContext()->renderer.get();
	renderer->renderModes[Lvn_RenderMode_2d].drawList.push_back(drawCmd);
}

} /* namespace lvn */
