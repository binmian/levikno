#include <levikno/levikno.h>

#include <vector>
#include <cstdint>

#define ARRAY_LEN(x) (sizeof(x) / sizeof(x[0]))


static float s_Vertices[] =
{
/*      pos (x,y,z)   |  TexCoord     */
	-0.5f, 0.5f, 0.0f, 0.0f, 1.0f, // v1
	-0.5f,-0.5f, 0.0f, 0.0f, 0.0f, // v2
	 0.5f, 0.5f, 0.0f, 1.0f, 1.0f, // v3
	 0.5f,-0.5f, 0.0f, 1.0f, 0.0f, // v4
};

static uint32_t s_Indices[] = 
{
	0, 1, 2, 2, 1, 3
};

static const char* s_VertexShaderSrc = R"(
#version 460

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 fragTexCoord;

void main()
{
	gl_Position = vec4(inPos, 1.0);
	fragTexCoord = inTexCoord;
}
)";

static const char* s_FragmentShaderSrc = R"(
#version 460

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec2 fragTexCoord;

layout(binding = 0) uniform sampler2D inTexture;

void main()
{
	vec3 color = vec3(texture(inTexture, fragTexCoord));
	outColor = vec4(color, 1.0);
}
)";


int main(int argc, char** argv)
{
	if (argc < 2)
	{
		printf("No input file given, please include a path to an image file\n");
		return -1;
	}

	// [Create Context]
	// create the context to load the library

	LvnContextCreateInfo lvnCreateInfo{};
	lvnCreateInfo.enableLogging = true;
	lvnCreateInfo.enableVulkanValidationLayers = true;
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
	windowInfo.title = "simpleTexture";
	windowInfo.width = 800;
	windowInfo.height = 600;
	windowInfo.minWidth = 300;
	windowInfo.minHeight = 200;

	LvnWindow* window;
	lvn::createWindow(&window, &windowInfo);


	// [Create Buffer]
	// create the buffer to store our vertex data

	// create the vertex attributes and descriptor bindings to layout our vertex data
	LvnVertexAttribute attributes[] = 
	{
		{ 0, 0, Lvn_VertexDataType_Vec3f, 0 },
		{ 0, 1, Lvn_VertexDataType_Vec2f, (3 * sizeof(float)) },
	};

	LvnVertexBindingDescription vertexBindingDescription{};
	vertexBindingDescription.binding = 0;
	vertexBindingDescription.stride = 5 * sizeof(float);

	// vertex buffer create info struct
	LvnBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.type = Lvn_BufferType_Vertex | Lvn_BufferType_Index;
	bufferCreateInfo.pVertexAttributes = attributes;
	bufferCreateInfo.vertexAttributeCount = ARRAY_LEN(attributes);
	bufferCreateInfo.pVertexBindingDescriptions = &vertexBindingDescription;
	bufferCreateInfo.vertexBindingDescriptionCount = 1;
	bufferCreateInfo.pVertices = s_Vertices;
	bufferCreateInfo.vertexBufferSize = sizeof(s_Vertices);
	bufferCreateInfo.pIndices = s_Indices;
	bufferCreateInfo.indexBufferSize = sizeof(s_Indices);

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
	LvnDescriptorBinding descriptorBinding{};
	descriptorBinding.binding = 0;
	descriptorBinding.descriptorType = Lvn_DescriptorType_CombinedImageSampler;
	descriptorBinding.shaderStage = Lvn_ShaderStage_Fragment;
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


	// [Create texture]

	// load image data
	LvnImageData imageData = lvn::loadImageData(argv[1], 4);

	// texture create info struct
	LvnTextureCreateInfo textureCreateInfo{};
	textureCreateInfo.imageData = imageData;
	textureCreateInfo.format = Lvn_TextureFormat_Unorm;
	textureCreateInfo.wrapMode = Lvn_TextureMode_Repeat;
	textureCreateInfo.minFilter = Lvn_TextureFilter_Linear;
	textureCreateInfo.magFilter = Lvn_TextureFilter_Linear;

	LvnTexture* texture;
	lvn::createTexture(&texture, &textureCreateInfo);

	// update descriptor set
	LvnDescriptorUpdateInfo descriptorUpdateInfo{};
	descriptorUpdateInfo.descriptorType = Lvn_DescriptorType_CombinedImageSampler;
	descriptorUpdateInfo.binding = 0;
	descriptorUpdateInfo.descriptorCount = 1;
	descriptorUpdateInfo.textureInfo = texture;

	lvn::updateDescriptorSetData(descriptorSet, &descriptorUpdateInfo, 1);


	// [Main Render Loop]
	while (lvn::windowOpen(window))
	{
		lvn::windowUpdate(window);

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

		// draw triangle
		lvn::renderCmdDrawIndexed(window, ARRAY_LEN(s_Indices)); // number of elements in indices array (3)

		// end render pass and submit rendering
		lvn::renderCmdEndRenderPass(window);
		lvn::renderEndCommandRecording(window);
		lvn::renderDrawSubmit(window); // note that this function is where we actually submit our render data to the GPU
	}

	// destroy objects after they are finished being used
	lvn::destroyTexture(texture);
	lvn::destroyBuffer(buffer);
	lvn::destroyPipeline(pipeline);
	lvn::destroyDescriptorLayout(descriptorLayout);
	lvn::destroyDescriptorSet(descriptorSet);
	lvn::destroyWindow(window);

	// terminate the context at the end of the program
	lvn::terminateContext();

	return 0;
}
