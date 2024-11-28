#include <levikno/levikno.h>

#include <sstream>
#include <string>
#include <vector>
#include <cstdint>
#include <chrono>


#define MAX_VERTEX_COUNT (5000)
#define MAX_INDEX_COUNT (5000)


#define ARRAY_LEN(x) (sizeof(x) / sizeof(x[0]))


static const char* s_VertexShaderSrc = R"(
#version 460

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

layout (binding = 0) uniform ObjectBuffer
{
	mat4 matrix;
} ubo;

void main()
{
	gl_Position = ubo.matrix * vec4(inPos, 0.0, 1.0);
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
	float text = texture(inTexture, fragTexCoord).r;
	outColor = vec4(vec3(text) * fragColor, 1.0);
}
)";

struct Vertex
{
	LvnVec2 pos;
	LvnVec3 color;
	LvnVec2 texUV;
};

struct DrawCommand
{
	Vertex* pVertices;
	uint32_t* pIndices;
	uint32_t vertexCount;
	uint32_t indexCount;
};

class DrawList
{
public:
	uint32_t vertexCount;
	uint32_t indexCount;
	uint64_t vertexSize;
	uint64_t indexSize;

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<DrawCommand> drawCommands;

	void push_back(const DrawCommand& drawCmd)
	{
		this->drawCommands.push_back(drawCmd);
		
		std::vector<uint32_t> batchIndices(drawCmd.pIndices, drawCmd.pIndices + drawCmd.indexCount);

		for (auto& index : batchIndices)
			index += this->vertexCount;

		this->vertices.insert(this->vertices.end(), drawCmd.pVertices, drawCmd.pVertices + drawCmd.vertexCount);
		this->indices.insert(this->indices.end(), batchIndices.begin(), batchIndices.end());

		this->vertexCount += drawCmd.vertexCount;
		this->indexCount += drawCmd.indexCount;
		this->vertexSize += drawCmd.vertexCount * sizeof(Vertex);
		this->indexSize += drawCmd.indexCount * sizeof(uint32_t);
	}

	void clear()
	{
		this->drawCommands.clear();
		this->vertices.clear();
		this->indices.clear();
		this->indexCount = 0;
		this->vertexCount = 0;
		this->indexSize = 0;
		this->vertexSize = 0;
	}

	bool empty()
	{
		if (this->drawCommands.empty() && this->vertices.empty() && this->indices.empty())
			return true;

		return false;
	}
};


class Timer
{
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_Time;

public:
	void start() { m_Time = std::chrono::high_resolution_clock::now(); }
	void reset() { m_Time = std::chrono::high_resolution_clock::now(); }
	float elapsed() { return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_Time).count() * 0.001f * 0.001f * 0.001f; }
	float elapsedms() { return elapsed() * 1000.0f; }
};
struct UniformData
{
	LvnMat4 matrix;
};


void drawText(DrawList* list, LvnFont* font, const char* text, LvnVec2 pos, LvnVec3 color, float scale)
{
	for (uint32_t i = 0; i < strlen(text); i++)
	{
		LvnFontGlyph glyph = lvn::fontGetGlyph(font, text[i]);

		float xpos = pos.x + glyph.bearing.x * scale;
		float ypos = pos.y + glyph.bearing.y * scale;

		float w = glyph.size.x * scale;
		float h = glyph.size.y * scale;

		Vertex rectVertices[] =
		{
			/*         pos         | color |         texUVs            */
			{ {xpos, ypos - h},      color, {glyph.uv.x0, glyph.uv.y1} },
			{ {xpos, ypos},          color, {glyph.uv.x0, glyph.uv.y0} },
			{ {xpos + w, ypos - h},  color, {glyph.uv.x1, glyph.uv.y1} },
			{ {xpos + w, ypos},      color, {glyph.uv.x1, glyph.uv.y0} },
		};

		uint32_t rectIndices[] =
		{
			0, 1, 2,
			2, 1, 3,
		};

		DrawCommand drawCmd{};
		drawCmd.pVertices = rectVertices;
		drawCmd.vertexCount = 4;
		drawCmd.pIndices = rectIndices;
		drawCmd.indexCount = 6;

		list->push_back(drawCmd);

		pos.x += glyph.advance * scale;
	}
}

int main(int argc, char** argv)
{
	// [Create Context]
	// create the context to load the library

	LvnContextCreateInfo lvnCreateInfo{};
	lvnCreateInfo.logging.enableLogging = true;
	lvnCreateInfo.logging.enableVulkanValidationLayers = true;
	lvnCreateInfo.windowapi = Lvn_WindowApi_glfw;
	lvnCreateInfo.graphicsapi = Lvn_GraphicsApi_vulkan;

	lvn::createContext(&lvnCreateInfo);


	// [Choose Device]
	// choose a physical device to render to

	uint32_t deviceCount = 0;
	std::vector<LvnPhysicalDevice*> devices;

	// first get number of devices, note that the first parameter is null
	lvn::getPhysicalDevices(nullptr, &deviceCount);

	// get an array of physical devices now that we know the number of devices
	devices.resize(deviceCount);
	lvn::getPhysicalDevices(devices.data(), &deviceCount);


	// initialize rendering, pass the physical device in the init struct
	LvnRenderInitInfo renderInfo{};

	// find and check if physical device is supported
	for (uint32_t i = 0; i < deviceCount; i++)
	{
		if (lvn::checkPhysicalDeviceSupport(devices[i]) == Lvn_Result_Success)
		{
			renderInfo.physicalDevice = devices[i];
			break;
		}
	}

	lvn::renderInit(&renderInfo);


	// window create info struct
	LvnWindowCreateInfo windowInfo{};
	windowInfo.title = "simpleText";
	windowInfo.width = 1280;
	windowInfo.height = 720;
	windowInfo.minWidth = 800;
	windowInfo.minHeight = 600;

	LvnWindow* window;
	lvn::createWindow(&window, &windowInfo);


	// [Create Buffer]
	// create the buffer to store our vertex data

	// create the vertex attributes and descriptor bindings to layout our vertex data
	LvnVertexAttribute attributes[] = 
	{
		{ 0, 0, Lvn_VertexDataType_Vec2f, 0 },
		{ 0, 1, Lvn_VertexDataType_Vec3f, (2 * sizeof(float)) },
		{ 0, 2, Lvn_VertexDataType_Vec2f, (5 * sizeof(float)) },
	};

	LvnVertexBindingDescription vertexBindingDescription{};
	vertexBindingDescription.binding = 0;
	vertexBindingDescription.stride = sizeof(Vertex);

	// vertex buffer create info struct
	LvnBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.type = Lvn_BufferType_DynamicVertex | Lvn_BufferType_DynamicIndex;
	bufferCreateInfo.pVertexAttributes = attributes;
	bufferCreateInfo.vertexAttributeCount = ARRAY_LEN(attributes);
	bufferCreateInfo.pVertexBindingDescriptions = &vertexBindingDescription;
	bufferCreateInfo.vertexBindingDescriptionCount = 1;
	bufferCreateInfo.pVertices = nullptr;
	bufferCreateInfo.vertexBufferSize = MAX_VERTEX_COUNT * sizeof(Vertex);
	bufferCreateInfo.pIndices = nullptr;
	bufferCreateInfo.indexBufferSize = MAX_INDEX_COUNT * sizeof(uint32_t);

	// create buffer
	LvnBuffer* buffer;
	lvn::createBuffer(&buffer, &bufferCreateInfo);


	// [Create Pipeline]
	// create the pipeline for how we want to render our scene

	// shader create info struct
	LvnShaderCreateInfo shaderCreateInfo{};
	shaderCreateInfo.vertexSrc = s_VertexShaderSrc;
	shaderCreateInfo.fragmentSrc = s_FragmentShaderSrc;

	// create shader from source
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
	descriptorTextureBinding.descriptorType = Lvn_DescriptorType_CombinedImageSampler;
	descriptorTextureBinding.shaderStage = Lvn_ShaderStage_Fragment;
	descriptorTextureBinding.descriptorCount = 1;
	descriptorTextureBinding.maxAllocations = 1;

	LvnDescriptorBinding descriptorBindings[] =
	{
		descriptorUniformBinding, descriptorTextureBinding,
	};

	// descriptor layout create info
	LvnDescriptorLayoutCreateInfo descriptorLayoutCreateInfo{};
	descriptorLayoutCreateInfo.pDescriptorBindings = descriptorBindings;
	descriptorLayoutCreateInfo.descriptorBindingCount = ARRAY_LEN(descriptorBindings);
	descriptorLayoutCreateInfo.maxSets = 1;

	// create descriptor layout
	LvnDescriptorLayout* descriptorLayout;
	lvn::createDescriptorLayout(&descriptorLayout, &descriptorLayoutCreateInfo);

	// create descriptor set using layout
	LvnDescriptorSet* descriptorSet;
	lvn::createDescriptorSet(&descriptorSet, descriptorLayout);


	// get the render pass from the window to pass into the pipeline
	LvnRenderPass* renderPass = lvn::windowGetRenderPass(window);

	// create pipeline specification or fixed functions
	LvnPipelineSpecification pipelineSpec = lvn::pipelineSpecificationGetConfig();

	// pipeline create info struct
	LvnPipelineCreateInfo pipelineCreateInfo{};
	pipelineCreateInfo.pipelineSpecification = &pipelineSpec;
	pipelineCreateInfo.pVertexAttributes = attributes;
	pipelineCreateInfo.vertexAttributeCount = ARRAY_LEN(attributes);
	pipelineCreateInfo.pVertexBindingDescriptions = &vertexBindingDescription;
	pipelineCreateInfo.vertexBindingDescriptionCount = 1;
	pipelineCreateInfo.pDescriptorLayouts = &descriptorLayout;
	pipelineCreateInfo.descriptorLayoutCount = 1;
	pipelineCreateInfo.shader = shader;
	pipelineCreateInfo.renderPass = renderPass;

	// create pipeline
	LvnPipeline* pipeline;
	lvn::createPipeline(&pipeline, &pipelineCreateInfo);

	// destroy the shader after creating the pipeline
	lvn::destroyShader(shader);


	// [Create uniform buffer]
	// uniform buffer create info struct
	LvnUniformBufferCreateInfo uniformBufferCreateInfo{};
	uniformBufferCreateInfo.type = Lvn_BufferType_Uniform;
	uniformBufferCreateInfo.binding = 0;
	uniformBufferCreateInfo.size = sizeof(UniformData);

	// create uniform buffer
	LvnUniformBuffer* uniformBuffer;
	lvn::createUniformBuffer(&uniformBuffer, &uniformBufferCreateInfo);


	// [Create texture]
	LvnFont font = lvn::loadFontFromFileTTF("res/fonts/JetBrainsMonoNerdFont-Regular.ttf", 16, {32, 126});


	LvnSamplerCreateInfo samplerCreateInfo{};
	samplerCreateInfo.wrapMode = Lvn_TextureMode_Repeat;
	samplerCreateInfo.minFilter = Lvn_TextureFilter_Linear;
	samplerCreateInfo.magFilter = Lvn_TextureFilter_Linear;

	LvnSampler* sampler;
	lvn::createSampler(&sampler, &samplerCreateInfo);

	// texture create info struct
	LvnTextureCreateInfo textureCreateInfo{};
	textureCreateInfo.imageData = font.atlas;
	textureCreateInfo.format = Lvn_TextureFormat_Unorm;
	textureCreateInfo.sampler = sampler;

	LvnTexture* texture;
	lvn::createTexture(&texture, &textureCreateInfo);

	// update descriptor set
	LvnDescriptorUpdateInfo descriptorUniformUpdateInfo{};
	descriptorUniformUpdateInfo.descriptorType = Lvn_DescriptorType_UniformBuffer;
	descriptorUniformUpdateInfo.binding = 0;
	descriptorUniformUpdateInfo.descriptorCount = 1;
	descriptorUniformUpdateInfo.bufferInfo = uniformBuffer;

	LvnDescriptorUpdateInfo descriptorTextureUpdateInfo{};
	descriptorTextureUpdateInfo.descriptorType = Lvn_DescriptorType_CombinedImageSampler;
	descriptorTextureUpdateInfo.binding = 1;
	descriptorTextureUpdateInfo.descriptorCount = 1;
	descriptorTextureUpdateInfo.pTextureInfos = &texture;

	LvnDescriptorUpdateInfo descriptorUpdateInfos[] =
	{
		descriptorUniformUpdateInfo, descriptorTextureUpdateInfo,
	};

	lvn::updateDescriptorSetData(descriptorSet, descriptorUpdateInfos, ARRAY_LEN(descriptorUpdateInfos));

	UniformData uniformData{};
	DrawList list{};

	Timer timer;
	timer.start();

	int progress = 0;

	std::string ascii;
	for (int i = 32; i <= 126; i++)
	{
		ascii += (char)i;
	}

	// [Main Render Loop]
	while (lvn::windowOpen(window))
	{
		lvn::windowUpdate(window);

		int width, height;
		lvn::windowGetSize(window, &width, &height);

		// update matrix
		LvnMat4 proj = lvn::ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);
		LvnMat4 view = LvnMat4(1.0f);
		LvnMat4 camera = proj * view;

		uniformData.matrix = camera;
		lvn::updateUniformBufferData(window, uniformBuffer, &uniformData, sizeof(UniformData));


		// text examples
		drawText(&list, &font, "hello world", {20.0f, height - 40.0f}, {1.0f, 1.0f, 1.0f}, 1.0f);
		drawText(&list, &font, "The quick brown fox jumps over the lazy dog", {20.0f, height - 80.0f}, {1.0f, 0.0f, 1.0f}, 1.0f);
		drawText(&list, &font, "this text changes color", {20.0f, height - 120.0f}, {0.0f, abs(sin(timer.elapsed())), abs(cos(timer.elapsed()))}, 1.0f);
		drawText(&list, &font, "<- this text moves ->", {120.0f + sin(timer.elapsed() * 2.0f) * 100.0f, height - 160.0f}, {0.0f, 1.0f, 1.0f}, 1.0f);
		drawText(&list, &font, "this text changes size (keep in mind the pixel quality when scaling)", {20.0f, height - 200.0f}, {1.0f, 1.0f, 0.0f}, abs(sin(timer.elapsed() * 0.5f)) * 0.5f + 0.5f);

		// progress bar text
		std::string str = "[";
		for (int i = 0; i < 100; i++)
		{
			str += (i <= progress ? "#" : ".");
		}
		str += "] ";
		str += std::to_string(progress);
		str += "%";

		progress = ceil(abs(sin(timer.elapsed() * 0.1f) * 100.0f));

		drawText(&list, &font, str.c_str(), {20.0f, height - 240.0f}, {1.0f, 0.3f, 0.0f}, 0.8f);
		str.clear();

		// window dimension text
		std::ostringstream ss;
		ss << "width: " << width << ", height: " << height;
		drawText(&list, &font, ss.str().c_str(), {20.0f, height - 280.0f}, {0.1f, 1.0f, 0.1f}, 1.0f);

		// mouse pos text
		float xpos, ypos;
		lvn::mouseGetPos(window, &xpos, &ypos);
		ss.str(""); ss.clear();
		ss << "mouse pos: (x:" << xpos << ",y:" << ypos << ")";
		drawText(&list, &font, ss.str().c_str(), {20.0f, height - 320.0f}, {0.1f, 1.0f, 0.1f}, 1.0f);

		// time text
		std::string time = std::to_string(timer.elapsed()) + " sec";
		drawText(&list, &font, time.c_str(), {20.0f, height - 360.0f}, {0.1f, 1.0f, 0.1f}, 1.0f);

		// ascii
		drawText(&list, &font, ascii.c_str(), {20.0f, height - 400.0f}, {1.0f, 1.0f, 1.0f}, 1.0f);


		// update buffers
		lvn::bufferUpdateVertexData(buffer, list.vertices.data(), list.vertices.size() * sizeof(Vertex), 0);
		lvn::bufferUpdateIndexData(buffer, list.indices.data(), list.indices.size() * sizeof(uint32_t), 0);

		// get next window swapchain image
		lvn::renderBeginNextFrame(window);
		lvn::renderBeginCommandRecording(window);

		// set background color and begin render pass
		lvn::renderClearColor(window, 0.0f, 0.0f, 0.0f, 1.0f);
		lvn::renderCmdBeginRenderPass(window);

		// bind pipeline
		lvn::renderCmdBindPipeline(window, pipeline);

		// bind descriptor set
		lvn::renderCmdBindDescriptorSets(window, pipeline, 0, 1, &descriptorSet);

		// bind vertex and index buffer
		lvn::renderCmdBindVertexBuffer(window, buffer);
		lvn::renderCmdBindIndexBuffer(window, buffer);

		lvn::renderCmdDrawIndexed(window, list.indices.size());

		// end render pass and submit rendering
		lvn::renderCmdEndRenderPass(window);
		lvn::renderEndCommandRecording(window);
		lvn::renderDrawSubmit(window); // note that this function is where we actually submit our render data to the GPU

		list.clear();
	}

	// destroy objects after they are finished being used
	lvn::destroySampler(sampler);
	lvn::destroyTexture(texture);
	lvn::destroyBuffer(buffer);
	lvn::destroyUniformBuffer(uniformBuffer);
	lvn::destroyPipeline(pipeline);
	lvn::destroyDescriptorLayout(descriptorLayout);
	lvn::destroyDescriptorSet(descriptorSet);
	lvn::destroyWindow(window);

	// terminate the context at the end of the program
	lvn::terminateContext();

	return 0;
}
