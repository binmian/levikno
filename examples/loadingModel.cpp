#include <levikno/levikno.h>

#include <vector>
#include <cstdint>
#include <chrono>

// INFO: this program loads a gltf model and render it on screen

#define ARRAY_LEN(x) (sizeof(x) / sizeof(x[0]))

#define MAX_OBJECTS 256


static const char* s_VertexShaderSrc = R"(
#version 460

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexUV;
layout(location = 3) in vec3 inNormals;
layout(location = 4) in vec3 inTangent;
layout(location = 5) in vec3 inBitangent;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragNormal;

struct ObjectData
{
	mat4 matrix;
	mat4 model;
};

layout(std140, binding = 0) readonly buffer UniformBufferObject
{
	ObjectData objects[];
} ubo;

void main()
{
	ObjectData obj = ubo.objects[gl_BaseInstance];
	gl_Position = obj.matrix * vec4(inPos, 1.0);
	fragColor = vec3(inColor);
	fragNormal = mat3(transpose(inverse(obj.model))) * inNormals;
}
)";

static const char* s_FragmentShaderSrc = R"(
#version 460

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragNormal;

layout(binding = 1) uniform UniformBuffer
{
	vec3 camPos;
	vec3 crntPos;
	vec3 lightPos;
	vec3 lightColor;
	float intensity;
	float specular;
} ubo;

void main()
{
	vec3 lightVec = ubo.lightPos - ubo.crntPos;
	float dist = length(lightVec);
	float a = 3.0f;
	float b = 0.7f;
	float inten = (ubo.intensity * 1.0f) / (a * dist * dist + b * dist + 1.0f);

	float ambient = 0.20f;

	vec3 normal = normalize(fragNormal);
	vec3 lightDirection = normalize(lightVec);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	float specularLight = ubo.specular;
	vec3 viewDirection = normalize(ubo.camPos - ubo.crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	vec3 result = (ambient + diffuse + specular) * fragColor;
	outColor = vec4(result, 1.0);
}
)";


struct UniformData
{
	LvnMat4 matrix;
	LvnMat4 model;
};

struct UniformLightData
{
	LvnVec3 camPos;
	alignas(16) LvnVec3 crntPos;
	alignas(16) LvnVec3 lightPos;
	alignas(16) LvnVec3 lightColor;
	float intensity;
	float specular;
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
	windowInfo.title = "loadingModel";
	windowInfo.width = 800;
	windowInfo.height = 600;
	windowInfo.minWidth = 300;
	windowInfo.minHeight = 200;

	LvnWindow* window;
	lvn::createWindow(&window, &windowInfo);


	// [Create Pipeline]
	// create the pipeline for how we want to render our scene

	LvnVertexAttribute attributes[6] =
	{
		{ 0, 0, Lvn_VertexDataType_Vec3f, 0 },                   // pos
		{ 0, 1, Lvn_VertexDataType_Vec4f, 3 * sizeof(float) },   // color
		{ 0, 2, Lvn_VertexDataType_Vec2f, 7 * sizeof(float) },   // texUV
		{ 0, 3, Lvn_VertexDataType_Vec3f, 9 * sizeof(float) },   // normal
		{ 0, 4, Lvn_VertexDataType_Vec3f, 12 * sizeof(float) },  // tangent
		{ 0, 5, Lvn_VertexDataType_Vec3f, 15 * sizeof(float) },  // bitangent
	};

	LvnVertexBindingDescription vertexBindingDescription{};
	vertexBindingDescription.stride = sizeof(LvnVertex);
	vertexBindingDescription.binding = 0;

	// shader create info struct
	LvnShaderCreateInfo shaderCreateInfo{};
	shaderCreateInfo.vertexSrc = s_VertexShaderSrc;
	shaderCreateInfo.fragmentSrc = s_FragmentShaderSrc;

	// create shader from source
	LvnShader* shader;
	lvn::createShaderFromSrc(&shader, &shaderCreateInfo);

	// descriptor binding
	LvnDescriptorBinding descriptorStorageBinding{};
	descriptorStorageBinding.binding = 0;
	descriptorStorageBinding.descriptorType = Lvn_DescriptorType_StorageBuffer;
	descriptorStorageBinding.shaderStage = Lvn_ShaderStage_Vertex;
	descriptorStorageBinding.descriptorCount = 1;
	descriptorStorageBinding.maxAllocations = 1;

	LvnDescriptorBinding descriptorUniformBinding{};
	descriptorUniformBinding.binding = 1;
	descriptorUniformBinding.descriptorType = Lvn_DescriptorType_UniformBuffer;
	descriptorUniformBinding.shaderStage = Lvn_ShaderStage_Fragment;
	descriptorUniformBinding.descriptorCount = 1;
	descriptorUniformBinding.maxAllocations = 1;

	LvnDescriptorBinding descriptorBindings[] =
	{
		descriptorStorageBinding, descriptorUniformBinding,
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
	pipelineSpec.depthstencil.enableDepth = true;
	pipelineSpec.depthstencil.depthOpCompare = Lvn_CompareOperation_LessOrEqual;

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
	// uniform storage create info struct
	LvnUniformBufferCreateInfo storageBufferCreateInfo{};
	storageBufferCreateInfo.type = Lvn_BufferType_Storage;
	storageBufferCreateInfo.binding = 0;
	storageBufferCreateInfo.size = sizeof(UniformData) * MAX_OBJECTS;

	// create storage buffer
	LvnUniformBuffer* storageBuffer;
	lvn::createUniformBuffer(&storageBuffer, &storageBufferCreateInfo);

	// uniform buffer create info struct
	LvnUniformBufferCreateInfo uniformBufferCreateInfo{};
	uniformBufferCreateInfo.type = Lvn_BufferType_Uniform;
	uniformBufferCreateInfo.binding = 1;
	uniformBufferCreateInfo.size = sizeof(UniformLightData);

	// create uniform buffer
	LvnUniformBuffer* uniformBuffer;
	lvn::createUniformBuffer(&uniformBuffer, &uniformBufferCreateInfo);

	// update descriptor set
	LvnDescriptorUpdateInfo descriptorStorageBufferUpdateInfo{};
	descriptorStorageBufferUpdateInfo.descriptorType = Lvn_DescriptorType_StorageBuffer;
	descriptorStorageBufferUpdateInfo.binding = 0;
	descriptorStorageBufferUpdateInfo.descriptorCount = 1;
	descriptorStorageBufferUpdateInfo.bufferInfo = storageBuffer;

	LvnDescriptorUpdateInfo descriptorUniformBufferUpdateInfo{};
	descriptorUniformBufferUpdateInfo.descriptorType = Lvn_DescriptorType_UniformBuffer;
	descriptorUniformBufferUpdateInfo.binding = 1;
	descriptorUniformBufferUpdateInfo.descriptorCount = 1;
	descriptorUniformBufferUpdateInfo.bufferInfo = uniformBuffer;

	LvnDescriptorUpdateInfo descriptorUpdateInfo[] =
	{
		descriptorStorageBufferUpdateInfo, descriptorUniformBufferUpdateInfo,
	};

	lvn::updateDescriptorSetData(descriptorSet, descriptorUpdateInfo, ARRAY_LEN(descriptorUpdateInfo));


	// [Load model]
	LvnModel model = lvn::loadModel("res/models/teapot.gltf");


	auto startTime = std::chrono::high_resolution_clock::now();

	std::vector<UniformData> uniformData; uniformData.resize(model.meshes.size());

	UniformLightData lightData{};

	// [Main Render Loop]
	while (lvn::windowOpen(window))
	{
		lvn::windowUpdate(window);

		int width, height;
		lvn::windowGetSize(window, &width, &height);

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();


		LvnVec3 camPos = { 5.0f, 3.0f, -5.0f };

		// update matrix
		LvnMat4 proj = lvn::perspective(lvn::radians(60.0f), (float)width / (float)height, 0.01f, 1000.0f);
		LvnMat4 view = lvn::lookAt(camPos, LvnVec3(0.0f, 0.0f, 0.0f), LvnVec3(0.0f, 1.0f, 0.0f));
		LvnMat4 modelMatrix = lvn::rotate(LvnMat4(1.0f), lvn::radians(time * 50.0f), LvnVec3(0.0f, 1.0f, 0.0f));
		LvnMat4 camera = proj * view * modelMatrix;

		for (uint32_t i = 0; i < model.meshes.size(); i++)
		{
			uniformData[i].matrix = camera * model.meshes[i].modelMatrix;
			uniformData[i].model = modelMatrix * model.meshes[i].modelMatrix;
		}

		lvn::updateUniformBufferData(window, storageBuffer, uniformData.data(), sizeof(UniformData) * uniformData.size());

		lightData.camPos = camPos;
		lightData.crntPos = { 0.0f, 0.0f, 0.0f };
		lightData.lightPos = { 5.0f, 5.0f, 0.0f };
		lightData.lightColor = { 1.0f, 1.0f, 1.0f };
		lightData.intensity = 10.0f;
		lightData.specular = 0.4f;

		lvn::updateUniformBufferData(window, uniformBuffer, &lightData, sizeof(UniformLightData));

		// get next window swapchain image
		lvn::renderBeginNextFrame(window);
		lvn::renderBeginCommandRecording(window);

		// set background color and begin render pass
		lvn::renderClearColor(window, 0.1f, 0.1f, 0.1f, 1.0f);
		lvn::renderCmdBeginRenderPass(window);

		// bind pipeline
		lvn::renderCmdBindPipeline(window, pipeline);

		// bind descriptor set
		lvn::renderCmdBindDescriptorSets(window, pipeline, 0, 1, &descriptorSet);

		// bind model vertex and index buffer
		for (uint32_t i = 0; i < model.meshes.size(); i++)
		{
			lvn::renderCmdBindVertexBuffer(window, lvn::meshGetBuffer(&model.meshes[i]));
			lvn::renderCmdBindIndexBuffer(window, lvn::meshGetBuffer(&model.meshes[i]));

			lvn::renderCmdDrawIndexedInstanced(window, model.meshes[i].indexCount, 1, i);
		}

		// end render pass and submit rendering
		lvn::renderCmdEndRenderPass(window);
		lvn::renderEndCommandRecording(window);
		lvn::renderDrawSubmit(window); // note that this function is where we actually submit our render data to the GPU
	}

	// destroy objects after they are finished being used
	lvn::freeModel(&model);
	lvn::destroyUniformBuffer(uniformBuffer);
	lvn::destroyUniformBuffer(storageBuffer);
	lvn::destroyPipeline(pipeline);
	lvn::destroyDescriptorLayout(descriptorLayout);
	lvn::destroyDescriptorSet(descriptorSet);
	lvn::destroyWindow(window);

	// terminate the context at the end of the program
	lvn::terminateContext();

	return 0;
}
