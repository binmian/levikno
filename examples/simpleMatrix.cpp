#include <levikno/levikno.h>

#define ARRAY_LEN(x) (sizeof(x) / sizeof(x[0]))


static float s_Vertices[] =
{
/*      pos (x,y,z)   |   color (r,g,b)  */
	-0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // v1
	-0.5f,-0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // v2
	 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // v3
	 0.5f,-0.5f, 0.0f, 1.0f, 0.0f, 1.0f, // v4
};

static uint32_t s_Indices[] = 
{
	0, 1, 2, 2, 1, 3
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


struct UniformData
{
	LvnMat4 matrix;
};


int main(int argc, char** argv)
{
	// [Create Context]
	// create the context to load the library

	LvnContextCreateInfo lvnCreateInfo{};
	lvnCreateInfo.logging.enableLogging = true;
	lvnCreateInfo.logging.enableGraphicsApiDebugLogging = true;
	lvnCreateInfo.windowapi = Lvn_WindowApi_glfw;
	lvnCreateInfo.graphicsapi = Lvn_GraphicsApi_vulkan;

	lvn::createContext(&lvnCreateInfo);


	// window create info struct
	LvnWindowCreateInfo windowInfo{};
	windowInfo.title = "simpleMatrix";
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
		{ 0, 0, Lvn_AttributeFormat_Vec3_f32, 0 },
		{ 0, 1, Lvn_AttributeFormat_Vec3_f32, (3 * sizeof(float)) },
	};

	LvnVertexBindingDescription vertexBindingDescription{};
	vertexBindingDescription.binding = 0;
	vertexBindingDescription.stride = 6 * sizeof(float);

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
	LvnDescriptorBinding descriptorUniformBinding{};
	descriptorUniformBinding.binding = 0;
	descriptorUniformBinding.descriptorType = Lvn_DescriptorType_UniformBuffer;
	descriptorUniformBinding.shaderStage = Lvn_ShaderStage_Vertex;
	descriptorUniformBinding.descriptorCount = 1;
	descriptorUniformBinding.maxAllocations = 1;

	// descriptor layout create info
	LvnDescriptorLayoutCreateInfo descriptorLayoutCreateInfo{};
	descriptorLayoutCreateInfo.pDescriptorBindings = &descriptorUniformBinding;
	descriptorLayoutCreateInfo.descriptorBindingCount = 1;
	descriptorLayoutCreateInfo.maxSets = 1;

	// create descriptor layout
	LvnDescriptorLayout* descriptorLayout;
	lvn::createDescriptorLayout(&descriptorLayout, &descriptorLayoutCreateInfo);

	// create descriptor set using layout
	LvnDescriptorSet* descriptorSet;
	lvn::allocateDescriptorSet(&descriptorSet, descriptorLayout);


	// get the render pass from the window to pass into the pipeline
	LvnRenderPass* renderPass = lvn::windowGetRenderPass(window);

	// create pipeline specification or fixed functions
	LvnPipelineSpecification pipelineSpec = lvn::configPipelineSpecificationInit();

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
	uniformBufferCreateInfo.size = sizeof(UniformData);

	// create uniform buffer
	LvnUniformBuffer* uniformBuffer;
	lvn::createUniformBuffer(&uniformBuffer, &uniformBufferCreateInfo);


	// update descriptor set
	LvnUniformBufferInfo bufferInfo{};
	bufferInfo.buffer = uniformBuffer;
	bufferInfo.range = sizeof(UniformData);
	bufferInfo.offset = 0;

	LvnDescriptorUpdateInfo descriptorUniformBufferUpdateInfo{};
	descriptorUniformBufferUpdateInfo.descriptorType = Lvn_DescriptorType_UniformBuffer;
	descriptorUniformBufferUpdateInfo.binding = 0;
	descriptorUniformBufferUpdateInfo.descriptorCount = 1;
	descriptorUniformBufferUpdateInfo.bufferInfo = &bufferInfo;

	lvn::updateDescriptorSetData(descriptorSet, &descriptorUniformBufferUpdateInfo, 1);

	UniformData uniformData{};

	// [Main Render Loop]
	while (lvn::windowOpen(window))
	{
		lvn::windowUpdate(window);

		int width, height;
		lvn::windowGetSize(window, &width, &height);

		// update matrix
		LvnMat4 proj = lvn::perspective(lvn::radians(60.0f), (float)width / (float)height, 0.01f, 1000.0f);
		LvnMat4 view = lvn::lookAt(LvnVec3(0.0f, 0.0f, -2.0f), LvnVec3(0.0f, 0.0f, 0.0f), LvnVec3(0.0f, 1.0f, 0.0f));
		LvnMat4 camera = proj * view;

		uniformData.matrix = camera;
		lvn::updateUniformBufferData(uniformBuffer, &uniformData, sizeof(UniformData), 0);

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

		// draw sqaure
		lvn::renderCmdDrawIndexed(window, ARRAY_LEN(s_Indices)); // number of elements in indices array (6)

		// end render pass and submit rendering
		lvn::renderCmdEndRenderPass(window);
		lvn::renderEndCommandRecording(window);
		lvn::renderDrawSubmit(window); // note that this function is where we actually submit our render data to the GPU
	}

	// destroy objects after they are finished being used
	lvn::destroyBuffer(buffer);
	lvn::destroyUniformBuffer(uniformBuffer);
	lvn::destroyPipeline(pipeline);
	lvn::destroyDescriptorLayout(descriptorLayout);
	lvn::destroyWindow(window);

	// terminate the context at the end of the program
	lvn::terminateContext();

	return 0;
}
