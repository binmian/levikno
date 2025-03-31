#include <levikno/levikno.h>

#include <vector>
#include <cstdint>
#include <chrono>

#define ARRAY_LEN(x) (sizeof(x) / sizeof(x[0]))

// INFO: this program demonstrates the use of framebuffers by using post processing shaders,
//       the scene is rendered to an offscreen framebuffer which is then stored on a texture
//       image, the image is rendered to the screen through the framebuffer shader for post
//       processing.

// shaders

// vertex shader
static const char* s_VertexShaderSrc = R"(
#version 460

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 fragTexCoord;

layout (binding = 0) uniform ObjectBuffer
{
	mat4 matrix;
} ubo;

void main()
{
	gl_Position = ubo.matrix * vec4(inPos, 1.0);
	fragTexCoord = inTexCoord;
}
)";

// fragment shader
static const char* s_FragmentShaderSrc = R"(
#version 460

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec2 fragTexCoord;

layout(binding = 1) uniform sampler2D inTexture;

void main()
{
	vec3 color = vec3(texture(inTexture, fragTexCoord));
	outColor = vec4(color, 1.0);
}
)";

// frame buffer shaders
// INFO: modify the frame buffer shaders below to change the output of the screen

// vertex shader
static const char* s_FbVertexShaderSrc = R"(
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

// fragment shader
static const char* s_FbFragmentShaderSrc = R"(
#version 460

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec2 fragTexCoord;

layout(binding = 1) uniform sampler2D texSampler;

void main()
{
	// [invert colors]
	outColor = vec4(vec3(1.0 - texture(texSampler, fragTexCoord)), 1.0);
	
	// [greyscale]
	// vec3 texColor = vec3(texture(texSampler, fragTexCoord));
	// float average = (texColor.r + texColor.g + texColor.b) / 3.0;
	// outColor = vec4(vec3(average), 1.0);
}
)";


struct UniformData
{
	LvnMat4 matrix;
};

struct EventData
{
	LvnDescriptorSet* fbDescriptorSet;
	LvnFrameBuffer* frameBuffer;
};


// NOTE: we update and resize the framebuffer when ever the window changes size through window events

bool windowFrameBufferResize(LvnWindowFramebufferResizeEvent* e, void* userData)
{
	EventData* data = static_cast<EventData*>(userData);

	lvn::frameBufferResize(data->frameBuffer, e->width, e->height);

	LvnDescriptorUpdateInfo fbDescriptorUpdateInfo;

	LvnTexture* framebufferImage = lvn::frameBufferGetImage(data->frameBuffer, 0);

	fbDescriptorUpdateInfo.descriptorType = Lvn_DescriptorType_ImageSampler;
	fbDescriptorUpdateInfo.binding = 1;
	fbDescriptorUpdateInfo.descriptorCount = 1;
	fbDescriptorUpdateInfo.pTextureInfos = &framebufferImage;

	lvn::updateDescriptorSetData(data->fbDescriptorSet, &fbDescriptorUpdateInfo, 1);

	return true;
}

void eventsCallbackFn(LvnEvent* e)
{
	lvn::dispatchWindowFramebufferResizeEvent(e, windowFrameBufferResize);
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
	windowInfo.title = "framebuffer";
	windowInfo.width = 800;
	windowInfo.height = 600;
	windowInfo.minWidth = 300;
	windowInfo.minHeight = 200;

	LvnWindow* window;
	lvn::createWindow(&window, &windowInfo);


	// [Create frame buffer]
	// find supported depth format
	LvnDepthImageFormat depthFormats[] =
	{
		Lvn_DepthImageFormat_Depth32Stencil8, Lvn_DepthImageFormat_Depth24Stencil8, Lvn_DepthImageFormat_Depth32, Lvn_DepthImageFormat_Depth16,
	};

	// NOTE: formats earlier in the array will have higher priority and will be checked first in that order
	LvnDepthImageFormat supportedDepthFormat = lvn::findSupportedDepthImageFormat(depthFormats, ARRAY_LEN(depthFormats));

	// frame buffer attachments
	LvnFrameBufferColorAttachment frameBufferColorAttachment = { 0, Lvn_ColorImageFormat_RGBA32F };
	LvnFrameBufferDepthAttachment frameBufferDepthAttachment = { 1, supportedDepthFormat }; // pass the supported format into the depth attachment

	// frame buffer create info struct
	LvnFrameBufferCreateInfo frameBufferCreateInfo{};
	frameBufferCreateInfo.width = 800;
	frameBufferCreateInfo.height = 600;
	frameBufferCreateInfo.sampleCount = Lvn_SampleCount_1_Bit;
	frameBufferCreateInfo.pColorAttachments = &frameBufferColorAttachment;
	frameBufferCreateInfo.colorAttachmentCount = 1;
	frameBufferCreateInfo.depthAttachment = &frameBufferDepthAttachment;
	frameBufferCreateInfo.textureMode = Lvn_TextureMode_ClampToEdge;
	frameBufferCreateInfo.textureFilter = Lvn_TextureFilter_Linear;

	// create framebuffer
	LvnFrameBuffer* frameBuffer;
	lvn::createFrameBuffer(&frameBuffer, &frameBufferCreateInfo);


	// [Create texture]
	// load image data
	LvnImageData imageData = lvn::loadImageData("res/images/woodBox.jpg", 4, true);

	// texture create info struct
	LvnTextureCreateInfo textureCreateInfo{};
	textureCreateInfo.imageData = imageData;
	textureCreateInfo.format = Lvn_TextureFormat_Unorm;
	textureCreateInfo.wrapS = Lvn_TextureMode_Repeat;
	textureCreateInfo.wrapT = Lvn_TextureMode_Repeat;
	textureCreateInfo.minFilter = Lvn_TextureFilter_Linear;
	textureCreateInfo.magFilter = Lvn_TextureFilter_Linear;

	LvnTexture* texture;
	lvn::createTexture(&texture, &textureCreateInfo);


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

	// NOTE: the width and height of the loaded image will be used for the size of our square in the vertex buffer
	float vertices[] =
	{
		/*    pos (x,y,z)    |      UV   */
		-0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,    1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,    0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,    1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,    1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,    1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,    0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,    0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,    1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,    1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,    1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,    1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,    1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,    0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,    0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,    0.0f, 1.0f
	};

	// vertex buffer create info struct
	LvnBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.type = Lvn_BufferType_Vertex;
	bufferCreateInfo.pVertexAttributes = attributes;
	bufferCreateInfo.vertexAttributeCount = ARRAY_LEN(attributes);
	bufferCreateInfo.pVertexBindingDescriptions = &vertexBindingDescription;
	bufferCreateInfo.vertexBindingDescriptionCount = 1;
	bufferCreateInfo.pVertices = vertices;
	bufferCreateInfo.vertexBufferSize = sizeof(vertices);
	bufferCreateInfo.pIndices = nullptr;
	bufferCreateInfo.indexBufferSize = 0;

	// create buffer
	LvnBuffer* buffer;
	lvn::createBuffer(&buffer, &bufferCreateInfo);

	// framebuffer vertex buffer
	float fbVertices[] =
	{
		/*    pos (x,y,z)   |      UV   */
		-1.0f, -1.0f, 0.0f,    0.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,    1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,    1.0f, 1.0f,
		 1.0f,  1.0f, 0.0f,    1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f,    0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f,    0.0f, 0.0f,
	};

	bufferCreateInfo.pVertices = fbVertices;
	bufferCreateInfo.vertexBufferSize = sizeof(fbVertices);

	// create framebuffer buffer
	LvnBuffer* fbBuffer;
	lvn::createBuffer(&fbBuffer, &bufferCreateInfo);



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
	lvn::allocateDescriptorSet(&descriptorSet, descriptorLayout);


	// create pipeline specification or fixed functions
	LvnPipelineSpecification pipelineSpec = lvn::pipelineSpecificationGetConfig();
	pipelineSpec.depthstencil.enableDepth = true;
	pipelineSpec.depthstencil.depthOpCompare = Lvn_CompareOp_LessOrEqual;

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
	pipelineCreateInfo.renderPass = lvn::frameBufferGetRenderPass(frameBuffer); // NOTE: get the render pass from the framebuffer using this function and pass to pipeline

	// create pipeline
	LvnPipeline* pipeline;
	lvn::createPipeline(&pipeline, &pipelineCreateInfo);

	// destroy the shader after creating the pipeline
	lvn::destroyShader(shader);


	// framebuffer pipeline
	LvnShaderCreateInfo fbShaderCreateInfo{};
	fbShaderCreateInfo.vertexSrc = s_FbVertexShaderSrc;
	fbShaderCreateInfo.fragmentSrc = s_FbFragmentShaderSrc;

	// framebuffer shader
	LvnShader* fbShader;
	lvn::createShaderFromSrc(&fbShader, &fbShaderCreateInfo);

	// framebuffer descriptor bindings
	LvnDescriptorLayoutCreateInfo fbDescriptorLayoutCreateInfo{};
	fbDescriptorLayoutCreateInfo.pDescriptorBindings = &descriptorBindingTexture;
	fbDescriptorLayoutCreateInfo.descriptorBindingCount = 1;
	fbDescriptorLayoutCreateInfo.maxSets = 1;

	// create framebuffer descriptor layout
	LvnDescriptorLayout* fbDescriptorLayout;
	lvn::createDescriptorLayout(&fbDescriptorLayout, &fbDescriptorLayoutCreateInfo);

	// create framebuffer descriptor set
	LvnDescriptorSet* fbDescriptorSet;
	lvn::allocateDescriptorSet(&fbDescriptorSet, fbDescriptorLayout);

	pipelineCreateInfo.pDescriptorLayouts = &fbDescriptorLayout;
	pipelineCreateInfo.pVertexBindingDescriptions = &vertexBindingDescription;
	pipelineCreateInfo.vertexBindingDescriptionCount = 1;
	pipelineCreateInfo.pVertexAttributes = attributes;
	pipelineCreateInfo.vertexAttributeCount = 2;
	pipelineCreateInfo.shader = fbShader;
	pipelineCreateInfo.renderPass = lvn::windowGetRenderPass(window); // NOTE: get the render pass from the window to pass into the pipeline

	LvnPipeline* fbPipeline;
	lvn::createPipeline(&fbPipeline, &pipelineCreateInfo);

	lvn::destroyShader(fbShader);

	// [Create uniform buffer]
	// uniform buffer create info struct
	LvnUniformBufferCreateInfo uniformBufferCreateInfo{};
	uniformBufferCreateInfo.type = Lvn_BufferType_Uniform;
	uniformBufferCreateInfo.binding = 0;
	uniformBufferCreateInfo.size = sizeof(UniformData);

	// create uniform buffer
	LvnUniformBuffer* uniformBuffer;
	lvn::createUniformBuffer(&uniformBuffer, &uniformBufferCreateInfo);

	LvnTexture* fbImage = lvn::frameBufferGetImage(frameBuffer, 0);


	// update descriptor set
	LvnUniformBufferInfo bufferInfo{};
	bufferInfo.buffer = uniformBuffer;
	bufferInfo.range = sizeof(UniformData);
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
	descriptorTextureUpdateInfo.pTextureInfos = &texture;

	LvnDescriptorUpdateInfo descriptorUpdateInfos[] =
	{
		descriptorUniformUpdateInfo, descriptorTextureUpdateInfo,
	};

	lvn::updateDescriptorSetData(descriptorSet, descriptorUpdateInfos, ARRAY_LEN(descriptorUpdateInfos));

	// update framebuffer descriptor set
	descriptorTextureUpdateInfo.pTextureInfos = &fbImage;

	lvn::updateDescriptorSetData(fbDescriptorSet, &descriptorTextureUpdateInfo, 1);

	UniformData uniformData{};

	auto startTime = std::chrono::high_resolution_clock::now();

	int width, height;

	EventData eventData{};
	eventData.fbDescriptorSet = fbDescriptorSet;
	eventData.frameBuffer = frameBuffer;

	lvn::windowSetEventCallback(window, eventsCallbackFn, &eventData);

	// [Main Render Loop]
	while (lvn::windowOpen(window))
	{
		lvn::windowUpdate(window);

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		lvn::windowGetSize(window, &width, &height);

		// update matrix
		LvnMat4 proj = lvn::perspective(lvn::radians(60.0f), (float)width / (float)height, 0.01f, 100.0f);
		LvnMat4 view = lvn::lookAt(lvn::vec3(0.0f, 2.0f, -2.0f), lvn::vec3(0.0f, 0.0f, 0.0f), lvn::vec3(0.0f, 1.0f, 0.0f));
		LvnMat4 model = lvn::rotate(LvnMat4(1.0f), lvn::radians(time * 10.0f), LvnVec3(0.0f, 1.0f, 0.0f));
		LvnMat4 camera = proj * view * model;

		uniformData.matrix = camera;
		lvn::updateUniformBufferData(window, uniformBuffer, &uniformData, sizeof(UniformData), 0);

		// get next window swapchain image
		lvn::renderBeginNextFrame(window);
		lvn::renderBeginCommandRecording(window);

		// begin framebuffer recording
		lvn::renderCmdBeginFrameBuffer(window, frameBuffer);
		lvn::frameBufferSetClearColor(frameBuffer, 0, 0.0f, 0.0f, 0.0f, 1.0f);

		// bind pipeline
		lvn::renderCmdBindPipeline(window, pipeline);

		// bind descriptor set
		lvn::renderCmdBindDescriptorSets(window, pipeline, 0, 1, &descriptorSet);

		// bind vertex buffer
		lvn::renderCmdBindVertexBuffer(window, buffer);

		// draw vertices
		lvn::renderCmdDraw(window, ARRAY_LEN(vertices));

		// end framebuffer recording
		lvn::renderCmdEndFrameBuffer(window, frameBuffer);


		// set background color and begin render pass
		lvn::renderClearColor(window, 0.0f, 0.0f, 0.0f, 1.0f);
		lvn::renderCmdBeginRenderPass(window);

		// bind framebuffer pipeline
		lvn::renderCmdBindPipeline(window, fbPipeline);

		// bind framebuffer descriptor set
		lvn::renderCmdBindDescriptorSets(window, fbPipeline, 0, 1, &fbDescriptorSet);

		// bind framebuffer vertex buffer
		lvn::renderCmdBindVertexBuffer(window, fbBuffer);

		// draw vertices
		lvn::renderCmdDraw(window, ARRAY_LEN(fbVertices));

		// end render pass and submit rendering
		lvn::renderCmdEndRenderPass(window);
		lvn::renderEndCommandRecording(window);
		lvn::renderDrawSubmit(window); // note that this function is where we actually submit our render data to the GPU
	}

	// destroy objects after they are finished being used
	lvn::destroyTexture(texture);
	lvn::destroyFrameBuffer(frameBuffer);
	lvn::destroyBuffer(buffer);
	lvn::destroyBuffer(fbBuffer);
	lvn::destroyUniformBuffer(uniformBuffer);
	lvn::destroyPipeline(pipeline);
	lvn::destroyPipeline(fbPipeline);
	lvn::destroyDescriptorLayout(descriptorLayout);
	lvn::destroyDescriptorLayout(fbDescriptorLayout);
	lvn::destroyWindow(window);

	// terminate the context at the end of the program
	lvn::terminateContext();

	return 0;
}
