#include <levikno/levikno.h>

#include <vector>
#include <cstdint>
#include <chrono>

#define ARRAY_LEN(x) (sizeof(x) / sizeof(x[0]))


const static float s_CubemapVertices[] =
{
	//   Coordinates
	-1.0f, -1.0f,  1.0f, // 0       7--------6
	 1.0f, -1.0f,  1.0f, // 1      /|       /|
	 1.0f, -1.0f, -1.0f, // 2     4--------5 |
	-1.0f, -1.0f, -1.0f, // 3     | |      | |
	-1.0f,  1.0f,  1.0f, // 4     | 3------|-2
	 1.0f,  1.0f,  1.0f, // 5     |/       |/
	 1.0f,  1.0f, -1.0f, // 6     0--------1
	-1.0f,  1.0f, -1.0f  // 7
};

const static uint32_t s_CubemapIndices[] =
{
	// Right
	6, 2, 1,
	6, 1, 5,
	// Left
	4, 0, 3,
	4, 3, 7,
	// Top
	5, 4, 7,
	5, 7, 6,
	// Bottom
	3, 0, 1,
	3, 1, 2,
	// Front
	5, 1, 0,
	5, 0, 4,
	// Back
	7, 3, 2,
	7, 2, 6
};

static const char* s_VertexShaderSrc = R"(
#version 460

layout (location = 0) in vec3 inPos;

layout (location = 0) out vec3 fragTexCoord;

layout (binding = 0) uniform ObjectBuffer 
{
	mat4 matrix;
} ubo;

void main()
{
	vec4 pos = ubo.matrix * vec4(inPos, 1.0);
    gl_Position = pos.xyww;
	fragTexCoord = vec3(inPos.x, inPos.y, inPos.z);
}
)";

static const char* s_FragmentShaderSrc = R"(
#version 460

layout (location = 0) out vec4 outColor;

layout (location = 0) in vec3 fragTexCoord;

layout (binding = 1) uniform samplerCube samplerCubeMap;

void main()
{
	outColor = texture(samplerCubeMap, fragTexCoord);
}
)";


struct UniformData
{
	LvnMat4 matrix;
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
	renderInfo.maxFramesInFlight = 1;

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
	windowInfo.title = "cubemap";
	windowInfo.width = 800;
	windowInfo.height = 600;
	windowInfo.minWidth = 300;
	windowInfo.minHeight = 200;

	LvnWindow* window;
	lvn::createWindow(&window, &windowInfo);


	// [Create Cubemap]
	// cubemap texture create info
	LvnCubemapCreateInfo cubemapCreateInfo{};
	cubemapCreateInfo.posx = lvn::loadImageData("res/cubemaps/sky/px.jpg", 4);
	cubemapCreateInfo.negx = lvn::loadImageData("res/cubemaps/sky/nx.jpg", 4);
	cubemapCreateInfo.posy = lvn::loadImageData("res/cubemaps/sky/py.jpg", 4);
	cubemapCreateInfo.negy = lvn::loadImageData("res/cubemaps/sky/ny.jpg", 4);
	cubemapCreateInfo.posz = lvn::loadImageData("res/cubemaps/sky/pz.jpg", 4);
	cubemapCreateInfo.negz = lvn::loadImageData("res/cubemaps/sky/nz.jpg", 4);

	// create cubemap
	LvnCubemap* cubemap;
	lvn::createCubemap(&cubemap, &cubemapCreateInfo);


	// [Create Buffer]
	// create the buffer to store our vertex data

	// create the vertex attributes and descriptor bindings to layout our vertex data
	LvnVertexAttribute attributes[] =
	{
		{ 0, 0, Lvn_VertexDataType_Vec3f, 0 },
	};

	LvnVertexBindingDescription vertexBindingDescription{};
	vertexBindingDescription.binding = 0;
	vertexBindingDescription.stride = 3 * sizeof(float);


	// vertex buffer create info struct
	LvnBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.type = Lvn_BufferType_Vertex | Lvn_BufferType_Index;
	bufferCreateInfo.pVertexAttributes = attributes;
	bufferCreateInfo.vertexAttributeCount = ARRAY_LEN(attributes);
	bufferCreateInfo.pVertexBindingDescriptions = &vertexBindingDescription;
	bufferCreateInfo.vertexBindingDescriptionCount = 1;
	bufferCreateInfo.pVertices = s_CubemapVertices;
	bufferCreateInfo.vertexBufferSize = sizeof(s_CubemapVertices);
	bufferCreateInfo.pIndices = s_CubemapIndices;
	bufferCreateInfo.indexBufferSize = sizeof(s_CubemapIndices);

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
	LvnDescriptorBinding descriptorBindingUniform{};
	descriptorBindingUniform.binding = 0;
	descriptorBindingUniform.descriptorType = Lvn_DescriptorType_UniformBuffer;
	descriptorBindingUniform.shaderStage = Lvn_ShaderStage_Vertex;
	descriptorBindingUniform.descriptorCount = 1;
	descriptorBindingUniform.maxAllocations = 1;

	LvnDescriptorBinding descriptorBindingTexture{};
	descriptorBindingTexture.binding = 1;
	descriptorBindingTexture.descriptorType = Lvn_DescriptorType_ImageSampler;
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
	descriptorLayoutCreateInfo.descriptorBindingCount = ARRAY_LEN(descriptorBindings);
	descriptorLayoutCreateInfo.maxSets = 1;

	// create descriptor layout
	LvnDescriptorLayout* descriptorLayout;
	lvn::createDescriptorLayout(&descriptorLayout, &descriptorLayoutCreateInfo);

	// create descriptor set using layout
	LvnDescriptorSet* descriptorSet;
	lvn::createDescriptorSet(&descriptorSet, descriptorLayout);


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
	pipelineCreateInfo.renderPass = lvn::windowGetRenderPass(window);

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


	// update descriptor set
	LvnUniformBufferInfo bufferInfo{};
	bufferInfo.buffer = uniformBuffer;
	bufferInfo.range = sizeof(UniformData);
	bufferInfo.offset = 0;

	LvnTexture* cubemapTexture = lvn::cubemapGetTextureData(cubemap); // get the texture data from our cubemap

	LvnDescriptorUpdateInfo descriptorUniformUpdateInfo{};
	descriptorUniformUpdateInfo.descriptorType = Lvn_DescriptorType_UniformBuffer;
	descriptorUniformUpdateInfo.binding = 0;
	descriptorUniformUpdateInfo.descriptorCount = 1;
	descriptorUniformUpdateInfo.bufferInfo = &bufferInfo;

	LvnDescriptorUpdateInfo descriptorTextureUpdateInfo{};
	descriptorTextureUpdateInfo.descriptorType = Lvn_DescriptorType_ImageSampler;
	descriptorTextureUpdateInfo.binding = 1;
	descriptorTextureUpdateInfo.descriptorCount = 1;
	descriptorTextureUpdateInfo.pTextureInfos = &cubemapTexture; // pass the cubemap texture into the descriptor update struct

	LvnDescriptorUpdateInfo descriptorUpdateInfos[] =
	{
		descriptorUniformUpdateInfo, descriptorTextureUpdateInfo,
	};

	lvn::updateDescriptorSetData(descriptorSet, descriptorUpdateInfos, ARRAY_LEN(descriptorUpdateInfos));


	UniformData uniformData{};

	Timer timer{};
	timer.start();

	// [Main Render Loop]
	while (lvn::windowOpen(window))
	{
		lvn::windowUpdate(window);

		int width, height;
		lvn::windowGetSize(window, &width, &height);

		float time = timer.elapsed();

		// update matrix
		LvnMat4 proj = lvn::perspective(lvn::radians(60.0f), (float)width / (float)height, 0.01f, 1000.0f);
		LvnMat4 view = lvn::lookAt(LvnVec3(0.0f, 0.0f, 0.0f), LvnVec3(cos(time), 0.5f * sin(0.5f * time), (sin(time))), LvnVec3(0.0f, 1.0f, 0.0f));

		view = LvnMat4(LvnMat3(view));

		uniformData.matrix = proj * view;

		lvn::updateUniformBufferData(window, uniformBuffer, &uniformData, sizeof(UniformData), 0);

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

		lvn::renderCmdDrawIndexed(window, ARRAY_LEN(s_CubemapIndices));

		// end render pass and submit rendering
		lvn::renderCmdEndRenderPass(window);
		lvn::renderEndCommandRecording(window);
		lvn::renderDrawSubmit(window); // note that this function is where we actually submit our render data to the GPU
	}

	// destroy objects after they are finished being used
	lvn::destroyCubemap(cubemap);
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
