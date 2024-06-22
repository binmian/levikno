#include <cstdlib>
#include <levikno/levikno.h>

#include <vector>
#include <chrono>
#include <cstdint>

#define COLOR_FG 0.78, 0.82, 1.0
#define COLOR_BG 0.12, 0.11, 0.18

#define MAX_VERTEX_COUNT (5000)
#define MAX_INDEX_COUNT (5000)

struct Vertex
{
	LvnVec2 pos;
	LvnVec3 color;
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

struct Box
{
	LvnVec2 pos, size;
	LvnVec3 color;
};

struct Ball
{
	LvnVec2 pos, size;
	LvnVec3 color;

	float velocity, angle;
};

static const char* s_VertexShaderSrc = R"(
#version 460

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;


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

static const char* s_FragmentShaderSrc = R"(
#version 460

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 fragColor;

void main()
{
	outColor = vec4(fragColor, 1.0);
}
)";


void drawRect(DrawList* list, LvnVec2 pos, LvnVec2 size, LvnVec3 color)
{
	Vertex vertices[] = 
	{
		{{ pos.x, pos.y + size.y },          color },
		{{ pos.x, pos.y },                   color },
		{{ pos.x + size.x, pos.y },          color },
		{{ pos.x + size.x, pos.y + size.y }, color },
	};

	uint32_t indices[] = { 0, 1, 2, 0, 2, 3 };

	DrawCommand drawCmd{};
	drawCmd.pVertices = vertices;
	drawCmd.vertexCount = 4;
	drawCmd.pIndices = indices;
	drawCmd.indexCount = 6;

	list->push_back(drawCmd);
}

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
	windowInfo.minWidth = 800;
	windowInfo.minHeight = 600;

	LvnWindow* window;
	lvn::createWindow(&window, &windowInfo);


	// create the vertex attributes and descriptor bindings to layout our vertex data
	LvnVertexAttribute attributes[2] = 
	{
		{ 0, 0, Lvn_VertexDataType_Vec2f, 0 },
		{ 0, 1, Lvn_VertexDataType_Vec3f, (2 * sizeof(float)) },
	};

	LvnVertexBindingDescription vertexBindingDescription{};
	vertexBindingDescription.binding = 0;
	vertexBindingDescription.stride = sizeof(Vertex);

	// vertex buffer create info struct
	LvnBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.type = Lvn_BufferType_DynamicVertex | Lvn_BufferType_DynamicIndex;
	bufferCreateInfo.pVertexAttributes = attributes;
	bufferCreateInfo.vertexAttributeCount = 2;
	bufferCreateInfo.pVertexBindingDescriptions = &vertexBindingDescription;
	bufferCreateInfo.vertexBindingDescriptionCount = 1;
	bufferCreateInfo.pVertices = nullptr;
	bufferCreateInfo.vertexBufferSize = MAX_VERTEX_COUNT * sizeof(Vertex);
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

	// descriptor binding
	LvnDescriptorBinding descriptorBinding{};
	descriptorBinding.binding = 0;
	descriptorBinding.descriptorType = Lvn_DescriptorType_UniformBuffer;
	descriptorBinding.shaderStage = Lvn_ShaderStage_Vertex;
	descriptorBinding.descriptorCount = 1;
	descriptorBinding.maxAllocations = 1;

	// descriptor layout create info
	LvnDescriptorLayoutCreateInfo descriptorLayoutCreateInfo{};
	descriptorLayoutCreateInfo.pDescriptorBindings = &descriptorBinding;
	descriptorLayoutCreateInfo.descriptorBindingCount = 1;
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
	pipelineSpec.rasterizer.cullMode = Lvn_CullFaceMode_Front;

	// pipeline create info struct
	LvnPipelineCreateInfo pipelineCreateInfo{};
	pipelineCreateInfo.pipelineSpecification = &pipelineSpec;
	pipelineCreateInfo.pVertexBindingDescriptions = &vertexBindingDescription;
	pipelineCreateInfo.vertexBindingDescriptionCount = 1;
	pipelineCreateInfo.pVertexAttributes = attributes;
	pipelineCreateInfo.vertexAttributeCount = 2;
	pipelineCreateInfo.pDescriptorLayouts = &descriptorLayout;
	pipelineCreateInfo.descriptorLayoutCount = 1;
	pipelineCreateInfo.shader = shader;
	pipelineCreateInfo.renderPass = renderPass;

	// create pipeline
	LvnPipeline* pipeline;
	lvn::createPipeline(&pipeline, &pipelineCreateInfo);

	// destroy the shader after creating the pipeline
	lvn::destroyShader(shader);


	// uniform buffer create info
	LvnUniformBufferCreateInfo uniformBufferInfo{};
	uniformBufferInfo.binding = 0;
	uniformBufferInfo.type = Lvn_BufferType_Uniform;
	uniformBufferInfo.size = sizeof(UniformData);

	// create uniform buffer
	LvnUniformBuffer* uniformBuffer;
	lvn::createUniformBuffer(&uniformBuffer, &uniformBufferInfo);


	// update descriptor set
	LvnDescriptorUpdateInfo descriptorUpdateInfo{};
	descriptorUpdateInfo.descriptorType = Lvn_DescriptorType_UniformBuffer;
	descriptorUpdateInfo.binding = 0;
	descriptorUpdateInfo.descriptorCount = 1;
	descriptorUpdateInfo.bufferInfo = uniformBuffer;

	lvn::updateDescriptorSetData(descriptorSet, &descriptorUpdateInfo, 1);


	DrawList list{};
	UniformData uniformData{};

	float oldTime = 0.0f;
	Timer deltaTime;
	deltaTime.start();

	int width, height;
	lvn::windowGetSize(window, &width, &height);

	float paddleWidth = 20.0f, paddleHeight = 120.0f;
	float paddleSpeed = 1000.0f;

	Box p1{};
	p1.pos = { width * -0.5f + 20.0f, 0.0f };
	p1.size = { paddleWidth, paddleHeight };
	p1.color = { 1.0f, 1.0f, 1.0f };

	Box p2{};
	p2.pos = { width * 0.5f - p2.size.x - 20.0f, 0.0f };
	p2.size = { paddleWidth, paddleHeight };
	p2.color = { 1.0f, 1.0f, 1.0f };


	float ballSpeed = 500.0f;
	Ball ball{};
	ball.pos = { 0.0f, 0.0f };
	ball.size = { 20.0f, 20.0f };
	ball.color = { 1.0f, 1.0f, 1.0f };
	ball.velocity = ballSpeed;
	ball.angle = 30.0f;

	bool ballSide = false;

	LVN_INFO("Use w and s to move the left paddle up and down. Use the up and down arrow keys to move the right paddle up and down. Have fun!");


	while (lvn::windowOpen(window))
	{
		lvn::windowUpdate(window);

		int width, height;
		lvn::windowGetSize(window, &width, &height);

		LvnMat4 proj = lvn::ortho((float)width * -0.5f, (float)width * 0.5f, (float)height * -0.5f, (float)height * 0.5f, -1.0f, 1.0f);
		LvnMat4 view = LvnMat4(1.0f);
		LvnMat4 camera = proj * view;

		uniformData.matrix = camera;
		lvn::updateUniformBufferData(window, uniformBuffer, &uniformData, sizeof(uniformData));


		float paddleSpeed = height;

		float halfWidth = width * 0.5f;
		float halfHeight = height * 0.5f;

		p1.pos.x = -halfWidth + 20.0f;
		p2.pos.x = halfWidth - p2.size.x - 20.0f;

		float timeNow = deltaTime.elapsed();
		float dt = timeNow - oldTime;
		oldTime = timeNow;

		if (lvn::keyPressed(window, Lvn_KeyCode_W))
		{
			p1.pos.y += paddleSpeed * dt;
		}
		if (lvn::keyPressed(window, Lvn_KeyCode_S))
		{
			p1.pos.y -= paddleSpeed * dt;
		}
		if (lvn::keyPressed(window, Lvn_KeyCode_Up))
		{
			p2.pos.y += paddleSpeed * dt;
		}
		if (lvn::keyPressed(window, Lvn_KeyCode_Down))
		{
			p2.pos.y -= paddleSpeed * dt;
		}

		if (p1.pos.y <= -halfHeight) { p1.pos.y = -halfHeight; }
		else if (p1.pos.y >= halfHeight - paddleHeight) { p1.pos.y = halfHeight - paddleHeight; }
		if (p2.pos.y <= -halfHeight) { p2.pos.y = -halfHeight; }
		else if (p2.pos.y >= halfHeight - paddleHeight) { p2.pos.y = halfHeight - paddleHeight; }


		// ball movement

		// calc next ball pos
		ball.velocity = ballSpeed * dt;
		ball.pos.x += ball.velocity * cos(lvn::radians(ball.angle));
		ball.pos.y += ball.velocity * sin(lvn::radians(ball.angle));

		// hit bottom side
		if (ball.pos.y <= -halfHeight)
		{
			ball.pos.y = -halfHeight;
			ball.angle *= -1;
		}

		// hit top side
		if (ball.pos.y >= halfHeight - ball.size.y)
		{
			ball.pos.y = halfHeight - ball.size.y;
			ball.angle *= -1;
		}

		// hit left side
		if (ball.pos.x <= -halfWidth - ball.size.x)
		{
			ball.pos = {0.0f, 0.0f};
			ball.angle = (rand() % 61) - 30.0f;
			ballSide = false;
			ballSpeed = 500.0f;
		}

		// hit right side
		if (ball.pos.x >= halfWidth)
		{
			ball.pos = {0.0f, 0.0f};
			ball.angle = 180.0f - ((rand() % 61) - 30.0f);
			ballSide = true;
			ballSpeed = 500.0f;
		}


		// hit left paddle
		if (ball.pos.x <= p1.pos.x + p1.size.x && ballSide &&
			(ball.pos.y >= p1.pos.y && ball.pos.y + ball.size.y <= p1.pos.y + p1.size.y))
		{
			ball.angle = 180.0f - ball.angle + (rand() % 61) - 30.0f;
			ballSpeed += 10.0f;
			lvn::clamp(ballSpeed, 500.0f, 1500.0f);
			ballSide = false;
		}

		// hit right paddle
		if (ball.pos.x + ball.size.x >= p2.pos.x && !ballSide &&
			(ball.pos.y >= p2.pos.y && ball.pos.y + ball.size.y <= p2.pos.y + p2.size.y))
		{
			ball.angle = (180.0f - ball.angle) + (rand() % 61) - 30.0f;
			ballSpeed += 10.0f;
			lvn::clamp(ballSpeed, 500.0f, 1500.0f);
			ballSide = true;
		}


		drawRect(&list, p1.pos, p1.size, p1.color);
		drawRect(&list, p2.pos, p2.size, p2.color);
		drawRect(&list, ball.pos, ball.size, ball.color);

		lvn::bufferUpdateVertexData(buffer, list.vertices.data(), list.vertices.size() * sizeof(Vertex), 0);
		lvn::bufferUpdateIndexData(buffer, list.indices.data(), list.indices.size() * sizeof(uint32_t), 0);

		// begin render
		lvn::renderBeginNextFrame(window);
		lvn::renderBeginCommandRecording(window);

		// set background color and begin render pass
		lvn::renderClearColor(window, 0.0f, 0.0f, 0.0f, 1.0f);
		lvn::renderCmdBeginRenderPass(window);

		// bind pipeline
		lvn::renderCmdBindPipeline(window, pipeline);
		lvn::renderCmdBindDescriptorSets(window, pipeline, 0, 1, &descriptorSet);

		// bind vertex and index buffer
		lvn::renderCmdBindVertexBuffer(window, buffer);
		lvn::renderCmdBindIndexBuffer(window, buffer);

		// draw triangle
		lvn::renderCmdDrawIndexed(window, list.indices.size()); // number of elements in indices array (3)


		// end render pass and submit rendering
		lvn::renderCmdEndRenderPass(window);
		lvn::renderEndCommandRecording(window);
		lvn::renderDrawSubmit(window); // note that this function is where we actually submit our render data to the GPU

		list.clear();
	}

	lvn::destroyBuffer(buffer);
	lvn::destroyPipeline(pipeline);
	lvn::destroyDescriptorLayout(descriptorLayout);
	lvn::destroyDescriptorSet(descriptorSet);
	lvn::destroyUniformBuffer(uniformBuffer);
	lvn::destroyWindow(window);

	lvn::terminateContext();

	return 0;
}
