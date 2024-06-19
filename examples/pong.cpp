#include <cstdint>
#include <iterator>
#include <levikno/levikno.h>

#include <vector>


#define MAX_VERTEX_COUNT (5000)
#define MAX_INDEX_COUNT (5000)

struct DrawCommand
{
	float* pVertices;
	uint32_t* pIndices;
	uint32_t vertexCount;
	uint32_t indexCount;
	uint32_t vertexStride;
};

class DrawList
{
public:
	uint32_t vertexCount;
	uint32_t indexCount;
	uint64_t vertexSize;
	uint64_t indexSize;

	std::vector<float> vertices;
	std::vector<uint32_t> indices;
	std::vector<DrawCommand> drawCommands;

	void push_back(const DrawCommand& drawCmd)
	{
		this->drawCommands.push_back(drawCmd);
		
		std::vector<uint32_t> batchIndices(drawCmd.pIndices, drawCmd.pIndices + drawCmd.indexCount);

		for (auto& index : batchIndices)
			index += this->vertexCount;

		this->vertices.insert(this->vertices.end(), drawCmd.pVertices, drawCmd.pVertices + drawCmd.vertexCount * drawCmd.vertexStride);
		this->indices.insert(this->indices.end(), batchIndices.begin(), batchIndices.end());

		this->vertexCount += drawCmd.vertexCount;
		this->indexCount += drawCmd.indexCount;
		this->vertexSize += drawCmd.vertexCount * drawCmd.vertexStride * sizeof(float);
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


static float s_Vertices[] = 
{
/*      Pos (x,y,z)   |   color (r,g,b)   */
	 0.0f,-0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // v1
	 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // v2
	-0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // v3
};

static float s_Vertices2[] = 
{
/*      Pos (x,y,z)   |   color (r,g,b)   */
	 0.0f,-0.8f, 0.0f, 1.0f, 1.0f, 1.0f, // v1
	 0.1f, 0.1f, 0.0f, 0.0f, 1.0f, 1.0f, // v2
	-0.1f, 0.1f, 0.0f, 0.0f, 1.0f, 1.0f, // v3
};

static uint32_t s_Indices[] = 
{
	0, 1, 2
};

static const char* s_VertexShaderSrc = R"(
#version 460

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main()
{
	gl_Position = vec4(inPos, 1.0);
	fragColor = inColor;
}
)";

static const char* s_FragmentShaderSrc = R"(
#version 460

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 fragColor;

void main()
{
	outColor = vec4(fragColor, 1.0);
}
)";


int main(int argc, char** argv)
{
	// create contex and render init
	LvnContextCreateInfo lvnCreateInfo{};
	lvnCreateInfo.enableLogging = true;
	lvnCreateInfo.enableVulkanValidationLayers = true;
	lvnCreateInfo.windowapi = Lvn_WindowApi_glfw;
	lvnCreateInfo.graphicsapi = Lvn_GraphicsApi_vulkan;

	lvn::createContext(&lvnCreateInfo);


	uint32_t deviceCount = 0;
	std::vector<LvnPhysicalDevice*> devices;

	lvn::getPhysicalDevices(nullptr, &deviceCount);

	devices.resize(deviceCount);
	lvn::getPhysicalDevices(devices.data(), &deviceCount);


	LvnRenderInitInfo renderInfo{};

	for (uint32_t i = 0; i < deviceCount; i++)
	{
		if (lvn::checkPhysicalDeviceSupport(devices[i]) == Lvn_Result_Success)
		{
			renderInfo.physicalDevice = devices[i];
			break;
		}
	}

	lvn::renderInit(&renderInfo);


	// create window
	LvnWindowCreateInfo windowInfo{};
	windowInfo.title = "Pong";
	windowInfo.width = 800;
	windowInfo.height = 600;
	windowInfo.minWidth = 300;
	windowInfo.minHeight = 200;

	LvnWindow* window;
	lvn::createWindow(&window, &windowInfo);

	DrawCommand drawCmd{};
	drawCmd.pVertices = s_Vertices;
	drawCmd.vertexCount = 3;
	drawCmd.pIndices = s_Indices;
	drawCmd.indexCount = sizeof(s_Indices) / sizeof(s_Indices[0]);
	drawCmd.vertexStride = 6;

	DrawList drawList{};

	drawList.push_back(drawCmd);

	drawCmd.pVertices = s_Vertices2;
	drawCmd.vertexCount = 3;

	drawList.push_back(drawCmd);

	LVN_INFO("vertex size: %u", drawList.vertexSize);
	LVN_INFO("index size: %u", drawList.indexSize);

	for (uint32_t i = 0; i < drawList.vertices.size(); i++)
	{
		LVN_INFO("%f", drawList.vertices[i]);
	}

	// create the vertex attributes and descriptor bindings to layout our vertex data
	LvnVertexAttribute attributes[2] = 
	{
		{ 0, 0, Lvn_VertexDataType_Vec3f, 0 },
		{ 0, 1, Lvn_VertexDataType_Vec3f, (3 * sizeof(float)) },
	};

	LvnVertexBindingDescription vertexBindingDescription{};
	vertexBindingDescription.binding = 0;
	vertexBindingDescription.stride = 6 * sizeof(float);

	// vertex buffer create info struct
	LvnBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.type = Lvn_BufferType_DynamicVertex | Lvn_BufferType_DynamicIndex;
	bufferCreateInfo.pVertexAttributes = attributes;
	bufferCreateInfo.vertexAttributeCount = 2;
	bufferCreateInfo.pVertexBindingDescriptions = &vertexBindingDescription;
	bufferCreateInfo.vertexBindingDescriptionCount = 1;
	bufferCreateInfo.pVertices = nullptr;
	bufferCreateInfo.vertexBufferSize = MAX_VERTEX_COUNT * 6 * sizeof(float);
	bufferCreateInfo.pIndices = nullptr;
	bufferCreateInfo.indexBufferSize = MAX_INDEX_COUNT * sizeof(uint32_t);

	// create buffer
	LvnBuffer* buffer;
	lvn::createBuffer(&buffer, &bufferCreateInfo);


	// shader create info struct
	LvnShaderCreateInfo shaderCreateInfo{};
	shaderCreateInfo.vertexSrc = s_VertexShaderSrc;
	shaderCreateInfo.fragmentSrc = s_FragmentShaderSrc;

	// create shader from source
	LvnShader* shader;
	lvn::createShaderFromSrc(&shader, &shaderCreateInfo);

	// get the render pass from the window to pass into the pipeline
	LvnRenderPass* renderPass = lvn::windowGetRenderPass(window);

	// create pipeline specification or fixed functions
	LvnPipelineSpecification pipelineSpec = lvn::pipelineSpecificationGetConfig();

	// pipeline create info struct
	LvnPipelineCreateInfo pipelineCreateInfo{};
	pipelineCreateInfo.pipelineSpecification = &pipelineSpec;
	pipelineCreateInfo.pVertexBindingDescriptions = &vertexBindingDescription;
	pipelineCreateInfo.vertexBindingDescriptionCount = 1;
	pipelineCreateInfo.pVertexAttributes = attributes;
	pipelineCreateInfo.vertexAttributeCount = 2;
	pipelineCreateInfo.shader = shader;
	pipelineCreateInfo.renderPass = renderPass;

	// create pipeline
	LvnPipeline* pipeline;
	lvn::createPipeline(&pipeline, &pipelineCreateInfo);

	// destroy the shader after creating the pipeline
	lvn::destroyShader(shader);


	lvn::bufferUpdateVertexData(buffer, drawList.vertices.data(), drawList.vertices.size() * sizeof(float), 0);
	lvn::bufferUpdateIndexData(buffer, drawList.indices.data(), drawList.indices.size() * sizeof(uint32_t), 0);

	while (lvn::windowOpen(window))
	{
		lvn::windowUpdate(window);

		lvn::renderBeginNextFrame(window);
		lvn::renderBeginCommandRecording(window);

		// set background color and begin render pass
		lvn::renderClearColor(window, 0.0f, 0.0f, 0.0f, 1.0f);
		lvn::renderCmdBeginRenderPass(window);

		// bind pipeline
		lvn::renderCmdBindPipeline(window, pipeline);

		// bind vertex and index buffer
		lvn::renderCmdBindVertexBuffer(window, buffer);
		lvn::renderCmdBindIndexBuffer(window, buffer);

		// draw triangle
		lvn::renderCmdDrawIndexed(window, 6); // number of elements in indices array (3)


		// end render pass and submit rendering
		lvn::renderCmdEndRenderPass(window);
		lvn::renderEndCommandRecording(window);
		lvn::renderDrawSubmit(window); // note that this function is where we actually submit our render data to the GPU
	}

	lvn::destroyBuffer(buffer);
	lvn::destroyPipeline(pipeline);
	lvn::destroyWindow(window);

	lvn::terminateContext();

	return 0;
}
