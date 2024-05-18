#include <levikno/levikno.h>

#include <chrono>
#include <string>
#include <vector>

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

bool windowMoved(LvnWindowMovedEvent* e)
{
	LVN_TRACE("%s: (x:%d,y:%d)", e->name, e->x, e->y);
	return true;
}

bool windowResize(LvnWindowResizeEvent* e)
{
	LVN_TRACE("%s: (x:%d,y:%d)", e->name, e->width, e->height);
	return true;
}

bool mousePos(LvnMouseMovedEvent* e)
{
	LVN_TRACE("%s: (x:%d,y:%d)", e->name, e->x, e->y);
	return true;
}

bool keyPress(LvnKeyPressedEvent* e)
{
	LVN_TRACE("%s: code: %d", e->name, e->keyCode);
	return true;
}

bool keyRelease(LvnKeyReleasedEvent* e)
{
	LVN_TRACE("%s: code: %d", e->name, e->keyCode);
	return true;
}

bool keyHold(LvnKeyHoldEvent* e)
{
	LVN_TRACE("%s: code: %d (%d)", e->name, e->keyCode, e->repeat);
	return true;
}

bool keyTyped(LvnKeyTypedEvent* e)
{
	LVN_TRACE("%s: key: %c", e->name, e->key);
	return true;
}

bool mousePress(LvnMouseButtonPressedEvent* e)
{
	LVN_TRACE("%s: button: %d", e->name, e->buttonCode);
	return true;
}

bool mouseScroll(LvnMouseScrolledEvent* e)
{
	LVN_TRACE("%s: scroll: (x:%f, y:%f)", e->name, e->x, e->y);
	return true;
}

void eventsCallbackFn(LvnEvent* e)
{
	lvn::dispatchKeyPressedEvent(e, keyPress);
	lvn::dispatchKeyHoldEvent(e, keyHold);
	lvn::dispatchKeyReleasedEvent(e, keyRelease);
	lvn::dispatchKeyTypedEvent(e, keyTyped);
	lvn::dispatchMouseMovedEvent(e, mousePos);
	lvn::dispatchMouseButtonPressedEvent(e, mousePress);
	lvn::dispatchMouseScrolledEvent(e, mouseScroll);
}

float vertices[] =
{
	/*      pos        |       color     |   TexUV    */
	-1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
	 1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
	-1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
};

std::vector<LvnVertex> lvnVertices = 
{
	{ {-1.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} },
	{ { 1.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} },
	{ { 1.0f,  1.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} },
	{ {-1.0f,  1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} },
};

uint32_t indices[] =
{
	0, 1, 2, 2, 3, 0,
};

struct UniformData
{
	lvn::mat4 matrix;
};

const float s_CameraSpeed = 1.0f;
float s_AngleX = LVN_PI * 1.5f;

// clanky and poorly made camera movement function, used for testing and looking around
void cameraMovment(LvnWindow* window, LvnCamera* camera, float dt)
{
	if (lvn::keyPressed(window, Lvn_KeyCode_W))
		camera->position += (camera->orientation * s_CameraSpeed) * dt;

	if (lvn::keyPressed(window, Lvn_KeyCode_A))
		camera->position += (lvn::normalize(lvn::cross(camera->orientation, camera->upVector)) * s_CameraSpeed) * dt;

	if (lvn::keyPressed(window, Lvn_KeyCode_S))
		camera->position += (-camera->orientation * s_CameraSpeed) * dt;

	if (lvn::keyPressed(window, Lvn_KeyCode_D))
		camera->position += (-lvn::normalize(lvn::cross(camera->orientation, camera->upVector)) * s_CameraSpeed) * dt;

	if (lvn::keyPressed(window, Lvn_KeyCode_Space))
		camera->position += (-camera->upVector * s_CameraSpeed) * dt;

	if (lvn::keyPressed(window, Lvn_KeyCode_LeftControl))
		camera->position += (camera->upVector * s_CameraSpeed) * dt;

	if (lvn::keyPressed(window, Lvn_KeyCode_Left))
	{
		s_AngleX += dt;
		camera->orientation.x = cos(s_AngleX);
		camera->orientation.z = sin(s_AngleX);
	}
	if (lvn::keyPressed(window, Lvn_KeyCode_Right))
	{
		s_AngleX -= dt;
		camera->orientation.x = cos(s_AngleX);
		camera->orientation.z = sin(s_AngleX);
	}
	if (lvn::keyPressed(window, Lvn_KeyCode_Up))
	{
		camera->orientation.y = lvn::clamp(camera->orientation.y - dt, -1.0f, 1.0f);
	}
	if (lvn::keyPressed(window, Lvn_KeyCode_Down))
	{
		camera->orientation.y = lvn::clamp(camera->orientation.y + dt, -1.0f, 1.0f);
	}
}

int main()
{
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

	for (uint32_t i = 0; i < deviceCount; i++)
	{
		LvnPhysicalDeviceInfo deviceInfo = lvn::getPhysicalDeviceInfo(devices[i]);
		LVN_TRACE("name: %s, version: %d", deviceInfo.name, deviceInfo.driverVersion);
	}

	LvnRenderInitInfo renderInfo{};
	renderInfo.physicalDevice = devices[0];
	lvn::renderInit(&renderInfo);


	LvnWindowCreateInfo windowInfo{};
	windowInfo.width = 800;
	windowInfo.height = 600;
	windowInfo.title = "window";
	windowInfo.fullscreen = false;
	windowInfo.resizable = true;
	windowInfo.minWidth = 300;
	windowInfo.minHeight = 200;
	windowInfo.maxWidth = -1;
	windowInfo.maxHeight = -1;
	windowInfo.pIcons = nullptr;
	windowInfo.iconCount = 0;

	LvnWindow* window;
	lvn::createWindow(&window, &windowInfo);
	lvn::setWindowEventCallback(window, eventsCallbackFn);


	LvnFrameBufferColorAttachment frameBufferColorAttachment = { 0, Lvn_ImageFormat_RGBA32F };
	LvnFrameBufferDepthAttachment frameBufferDepthAttachment = { 1, Lvn_ImageFormat_Depth32Stencil8 };

	LvnFrameBufferCreateInfo frameBufferCreateInfo{};
	frameBufferCreateInfo.width = 800;
	frameBufferCreateInfo.height = 600;
	frameBufferCreateInfo.sampleCount = Lvn_SampleCount_8_Bit;
	frameBufferCreateInfo.pColorAttachments = &frameBufferColorAttachment;
	frameBufferCreateInfo.colorAttachmentCount = 1;
	frameBufferCreateInfo.depthAttachment = &frameBufferDepthAttachment;
	frameBufferCreateInfo.textureMode = Lvn_TextureMode_ClampToEdge;
	frameBufferCreateInfo.textureFilter = Lvn_TextureFilter_Linear;

	LvnFrameBuffer* frameBuffer;
	lvn::createFrameBuffer(&frameBuffer, &frameBufferCreateInfo);

	LvnShaderCreateInfo shaderCreateInfo{};
	shaderCreateInfo.vertexSrc = "/home/bma/Documents/dev/levikno/LeviknoEditor/res/shaders/vkvert.spv";
	shaderCreateInfo.fragmentSrc = "/home/bma/Documents/dev/levikno/LeviknoEditor/res/shaders/vkfrag.spv";

	LvnShader* shader;
	lvn::createShaderFromFileBin(&shader, &shaderCreateInfo);


	LvnVertexBindingDescription vertexBindingDescroption{};
	vertexBindingDescroption.stride = 8 * sizeof(float);
	vertexBindingDescroption.binding = 0;

	LvnVertexBindingDescription lvnVertexBindingDescription{};
	lvnVertexBindingDescription.stride = sizeof(LvnVertex);
	lvnVertexBindingDescription.binding = 0;

	LvnVertexAttribute attributes[3] = 
	{
		{ 0, 0, Lvn_VertexDataType_Vec3f, 0 },
		{ 0, 1, Lvn_VertexDataType_Vec3f, (3 * sizeof(float)) },
		{ 0, 2, Lvn_VertexDataType_Vec2f, (6 * sizeof(float)) },
	};

	LvnVertexAttribute lvnAttributes[6] = 
	{
		{ 0, 0, Lvn_VertexDataType_Vec3f, 0 },                   // pos
		{ 0, 1, Lvn_VertexDataType_Vec4f, 3 * sizeof(float) },   // color
		{ 0, 2, Lvn_VertexDataType_Vec2f, 7 * sizeof(float) },   // texUV
		{ 0, 3, Lvn_VertexDataType_Vec3f, 9 * sizeof(float) },   // normal
		{ 0, 4, Lvn_VertexDataType_Vec3f, 12 * sizeof(float) },  // tangent
		{ 0, 5, Lvn_VertexDataType_Vec3f, 15 * sizeof(float) },  // bitangent
	};

	LvnDescriptorBinding uniformDescriptorBinding{};
	uniformDescriptorBinding.binding = 0;
	uniformDescriptorBinding.descriptorType = Lvn_DescriptorType_UniformBuffer;
	uniformDescriptorBinding.shaderStage = Lvn_ShaderStage_Vertex;
	uniformDescriptorBinding.descriptorCount = 1;

	LvnDescriptorBinding combinedImageDescriptorBinding{};
	combinedImageDescriptorBinding.binding = 1;
	combinedImageDescriptorBinding.descriptorType = Lvn_DescriptorType_CombinedImageSampler;
	combinedImageDescriptorBinding.shaderStage = Lvn_ShaderStage_Fragment;
	combinedImageDescriptorBinding.descriptorCount = 1;

	std::vector<LvnDescriptorBinding> descriptorBindings = 
	{
		uniformDescriptorBinding, combinedImageDescriptorBinding,
	};

	LvnDescriptorLayoutCreateInfo descriptorLayoutCreateInfo{};
	descriptorLayoutCreateInfo.pDescriptorBindings = descriptorBindings.data();
	descriptorLayoutCreateInfo.descriptorBindingCount = descriptorBindings.size();

	LvnDescriptorLayout* descriptorLayout;
	lvn::createDescriptorLayout(&descriptorLayout, &descriptorLayoutCreateInfo);


	LvnPipelineSpecification pipelineSpec = lvn::getDefaultPipelineSpecification();
	pipelineSpec.depthstencil.enableDepth = true;
	pipelineSpec.depthstencil.depthOpCompare = Lvn_CompareOperation_Less;

	LvnPipelineCreateInfo pipelineCreateInfo{};
	pipelineCreateInfo.pipelineSpecification = &pipelineSpec;
	pipelineCreateInfo.pVertexBindingDescriptions = &vertexBindingDescroption;
	pipelineCreateInfo.vertexBindingDescriptionCount = 1;
	pipelineCreateInfo.pVertexAttributes = attributes;
	pipelineCreateInfo.vertexAttributeCount = 3;
	pipelineCreateInfo.pDescriptorLayouts = &descriptorLayout;
	pipelineCreateInfo.descriptorLayoutCount = 1;
	pipelineCreateInfo.shader = shader;
	pipelineCreateInfo.renderPass = lvn::getWindowRenderPass(window);

	LvnPipeline* pipeline;
	lvn::createPipeline(&pipeline, &pipelineCreateInfo);

	lvn::destroyShader(shader);


	// framebuffer pipeline
	LvnShaderCreateInfo fbShaderCreateInfo{};
	fbShaderCreateInfo.vertexSrc = "/home/bma/Documents/dev/levikno/LeviknoEditor/res/shaders/vkFBvert.spv";
	fbShaderCreateInfo.fragmentSrc = "/home/bma/Documents/dev/levikno/LeviknoEditor/res/shaders/vkFBfrag.spv";

	LvnShader* fbShader;
	lvn::createShaderFromFileBin(&fbShader, &fbShaderCreateInfo);

	std::vector<LvnDescriptorBinding> fbDescriptorBinding =
	{
		uniformDescriptorBinding, combinedImageDescriptorBinding,
	};

	LvnDescriptorLayoutCreateInfo fbDescriptorLayoutCreateInfo{};
	fbDescriptorLayoutCreateInfo.pDescriptorBindings = fbDescriptorBinding.data();
	fbDescriptorLayoutCreateInfo.descriptorBindingCount = fbDescriptorBinding.size();

	LvnDescriptorLayout* fbDescriptorLayout;
	lvn::createDescriptorLayout(&fbDescriptorLayout, &fbDescriptorLayoutCreateInfo);

	pipelineCreateInfo.pDescriptorLayouts = &fbDescriptorLayout;
	pipelineCreateInfo.pVertexBindingDescriptions = &lvnVertexBindingDescription;
	pipelineCreateInfo.vertexBindingDescriptionCount = 1;
	pipelineCreateInfo.pVertexAttributes = lvnAttributes;
	pipelineCreateInfo.vertexAttributeCount = 6;
	pipelineCreateInfo.shader = fbShader;
	pipelineCreateInfo.renderPass = lvn::getFrameBufferRenderPass(frameBuffer);
	pipelineCreateInfo.pipelineSpecification->multisampling.rasterizationSamples = Lvn_SampleCount_8_Bit;

	LvnPipeline* fbPipeline;
	lvn::createPipeline(&fbPipeline, &pipelineCreateInfo);

	lvn::destroyShader(fbShader);


	// cubemap pipeline
	LvnVertexBindingDescription cubemapBindingDescription{};
	cubemapBindingDescription.stride = 3 * sizeof(float);
	cubemapBindingDescription.binding = 0;
	
	LvnVertexAttribute cubemapAttributes[1] = 
	{
		{ 0, 0, Lvn_VertexDataType_Vec3f, 0 },
	};
	
	LvnShaderCreateInfo cubemapShaderCreateInfo{};
	cubemapShaderCreateInfo.vertexSrc = "/home/bma/Documents/dev/levikno/LeviknoEditor/res/shaders/vkCubeVert.spv";
	cubemapShaderCreateInfo.fragmentSrc = "/home/bma/Documents/dev/levikno/LeviknoEditor/res/shaders/vkCubeFrag.spv";
	
	LvnShader* cubemapShader;
	lvn::createShaderFromFileBin(&cubemapShader, &cubemapShaderCreateInfo);
	
	std::vector<LvnDescriptorBinding> cubemapDescriptorBinding =
	{
		uniformDescriptorBinding, combinedImageDescriptorBinding,
	};
	
	LvnDescriptorLayoutCreateInfo cubemapDescriptorLayoutCreateInfo{};
	cubemapDescriptorLayoutCreateInfo.pDescriptorBindings = cubemapDescriptorBinding.data();
	cubemapDescriptorLayoutCreateInfo.descriptorBindingCount = cubemapDescriptorBinding.size();
	
	LvnDescriptorLayout* cubemapDescriptorLayout;
	lvn::createDescriptorLayout(&cubemapDescriptorLayout, &cubemapDescriptorLayoutCreateInfo);
	pipelineCreateInfo.pDescriptorLayouts = &cubemapDescriptorLayout;
	pipelineCreateInfo.pVertexBindingDescriptions = &cubemapBindingDescription;
	pipelineCreateInfo.vertexBindingDescriptionCount = 1;
	pipelineCreateInfo.pVertexAttributes = cubemapAttributes;
	pipelineCreateInfo.vertexAttributeCount = 1;
	pipelineCreateInfo.shader = cubemapShader;
	pipelineCreateInfo.renderPass = lvn::getFrameBufferRenderPass(frameBuffer);
	pipelineCreateInfo.pipelineSpecification->depthstencil.depthOpCompare = Lvn_CompareOperation_LessOrEqual;
	
	LvnPipeline* cubemapPipeline;
	lvn::createPipeline(&cubemapPipeline, &pipelineCreateInfo);

	lvn::destroyShader(cubemapShader);


	// vertex/index buffer
	LvnBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.type = Lvn_BufferType_Vertex | Lvn_BufferType_Index;
	bufferCreateInfo.pVertexBindingDescriptions = &vertexBindingDescroption;
	bufferCreateInfo.vertexBindingDescriptionCount = 1;
	bufferCreateInfo.pVertexAttributes = attributes;
	bufferCreateInfo.vertexAttributeCount = 3;

	bufferCreateInfo.pVertices = vertices;
	bufferCreateInfo.vertexBufferSize = sizeof(vertices);

	bufferCreateInfo.pIndices = indices;
	bufferCreateInfo.indexBufferSize = sizeof(indices);

	LvnBuffer* buffer;
	lvn::createBuffer(&buffer, &bufferCreateInfo);

	LvnBufferCreateInfo cubemapBufferCreateInfo{};
	cubemapBufferCreateInfo.type = Lvn_BufferType_Vertex | Lvn_BufferType_Index;
	cubemapBufferCreateInfo.pVertexBindingDescriptions = &cubemapBindingDescription;
	cubemapBufferCreateInfo.vertexBindingDescriptionCount = 1;
	cubemapBufferCreateInfo.pVertexAttributes = cubemapAttributes;
	cubemapBufferCreateInfo.vertexAttributeCount = 1;

	cubemapBufferCreateInfo.pVertices = s_CubemapVertices;
	cubemapBufferCreateInfo.vertexBufferSize = sizeof(s_CubemapVertices);

	cubemapBufferCreateInfo.pIndices = s_CubemapIndices;
	cubemapBufferCreateInfo.indexBufferSize = sizeof(s_CubemapIndices);

	LvnBuffer* cubemapBuffer;
	lvn::createBuffer(&cubemapBuffer, &cubemapBufferCreateInfo);


	// uniform buffer
	LvnUniformBufferCreateInfo uniformBufferInfo{};
	uniformBufferInfo.binding = 0;
	uniformBufferInfo.type = Lvn_BufferType_Uniform;
	uniformBufferInfo.size = sizeof(UniformData);

	LvnUniformBuffer* uniformBuffer;
	lvn::createUniformBuffer(&uniformBuffer, &uniformBufferInfo);


	LvnUniformBufferCreateInfo fbUniformBufferInfo{};
	fbUniformBufferInfo.binding = 0;
	fbUniformBufferInfo.type = Lvn_BufferType_Uniform;
	fbUniformBufferInfo.size = sizeof(UniformData);

	LvnUniformBuffer* fbUniformBuffer;
	lvn::createUniformBuffer(&fbUniformBuffer, &fbUniformBufferInfo);


	LvnUniformBufferCreateInfo cubemapUniformBufferInfo{};
	cubemapUniformBufferInfo.binding = 0;
	cubemapUniformBufferInfo.type = Lvn_BufferType_Uniform;
	cubemapUniformBufferInfo.size = sizeof(UniformData);

	LvnUniformBuffer* cubemapUniformBuffer;
	lvn::createUniformBuffer(&cubemapUniformBuffer, &cubemapUniformBufferInfo);

	// texture
	LvnTextureCreateInfo textureCreateInfo{};
	textureCreateInfo.filepath = "/home/bma/Documents/dev/levikno/LeviknoEditor/res/images/grass.png";
	textureCreateInfo.binding = 1;
	textureCreateInfo.minFilter = Lvn_TextureFilter_Linear;
	textureCreateInfo.magFilter = Lvn_TextureFilter_Linear;
	textureCreateInfo.wrapMode = Lvn_TextureMode_Repeat;

	LvnTexture* texture;
	lvn::createTexture(&texture, &textureCreateInfo);

	// cubemap
	LvnCubemapCreateInfo cubemapCreateInfo{};
	cubemapCreateInfo.posx = lvn::loadImageData("/home/bma/Documents/dev/levikno/LeviknoEditor/res/cubemaps/space/right.png", 4);
	cubemapCreateInfo.negx = lvn::loadImageData("/home/bma/Documents/dev/levikno/LeviknoEditor/res/cubemaps/space/left.png", 4);
	cubemapCreateInfo.posy = lvn::loadImageData("/home/bma/Documents/dev/levikno/LeviknoEditor/res/cubemaps/space/top.png", 4);
	cubemapCreateInfo.negy = lvn::loadImageData("/home/bma/Documents/dev/levikno/LeviknoEditor/res/cubemaps/space/bottom.png", 4);
	cubemapCreateInfo.posz = lvn::loadImageData("/home/bma/Documents/dev/levikno/LeviknoEditor/res/cubemaps/space/front.png", 4);
	cubemapCreateInfo.negz = lvn::loadImageData("/home/bma/Documents/dev/levikno/LeviknoEditor/res/cubemaps/space/back.png", 4);

	LvnCubemap* cubemap;
	lvn::createCubemap(&cubemap, &cubemapCreateInfo);

	// update descriptor sets
	LvnDescriptorUpdateInfo descriptorUniformUpdateInfo{};
	descriptorUniformUpdateInfo.descriptorType = Lvn_DescriptorType_UniformBuffer;
	descriptorUniformUpdateInfo.binding = 0;
	descriptorUniformUpdateInfo.descriptorCount = 1;
	descriptorUniformUpdateInfo.bufferInfo = uniformBuffer;

	LvnDescriptorUpdateInfo descriptorTextureUpdateInfo{};
	descriptorTextureUpdateInfo.descriptorType = Lvn_DescriptorType_CombinedImageSampler;
	descriptorTextureUpdateInfo.binding = 1;
	descriptorTextureUpdateInfo.descriptorCount = 1;
	descriptorTextureUpdateInfo.textureInfo = texture;

	std::vector<LvnDescriptorUpdateInfo> descriptorUpdateInfo =
	{
		descriptorUniformUpdateInfo, descriptorTextureUpdateInfo,
	};

	lvn::updateDescriptorLayoutData(descriptorLayout, descriptorUpdateInfo.data(), descriptorUpdateInfo.size());

	LvnDescriptorUpdateInfo fbDescriptorUniformUpdateInfo{};
	fbDescriptorUniformUpdateInfo.descriptorType = Lvn_DescriptorType_UniformBuffer;
	fbDescriptorUniformUpdateInfo.binding = 0;
	fbDescriptorUniformUpdateInfo.descriptorCount = 1;
	fbDescriptorUniformUpdateInfo.bufferInfo = fbUniformBuffer;

	LvnDescriptorUpdateInfo fbDescriptorTextureUpdateInfo{};
	fbDescriptorTextureUpdateInfo.descriptorType = Lvn_DescriptorType_CombinedImageSampler;
	fbDescriptorTextureUpdateInfo.binding = 1;
	fbDescriptorTextureUpdateInfo.descriptorCount = 1;
	fbDescriptorTextureUpdateInfo.textureInfo = lvn::getFrameBufferImage(frameBuffer, 0);

	std::vector<LvnDescriptorUpdateInfo> fbDescriptorUpdateInfo =
	{
		fbDescriptorUniformUpdateInfo, fbDescriptorTextureUpdateInfo,
	};

	lvn::updateDescriptorLayoutData(fbDescriptorLayout, fbDescriptorUpdateInfo.data(), fbDescriptorUpdateInfo.size());

	LvnDescriptorUpdateInfo cubemapDescriptorUniformUpdateInfo{};
	cubemapDescriptorUniformUpdateInfo.descriptorType = Lvn_DescriptorType_UniformBuffer;
	cubemapDescriptorUniformUpdateInfo.binding = 0;
	cubemapDescriptorUniformUpdateInfo.descriptorCount = 1;
	cubemapDescriptorUniformUpdateInfo.bufferInfo = cubemapUniformBuffer;

	LvnDescriptorUpdateInfo cubemapDescriptorTextureUpdateInfo{};
	cubemapDescriptorTextureUpdateInfo.descriptorType = Lvn_DescriptorType_CombinedImageSampler;
	cubemapDescriptorTextureUpdateInfo.binding = 1;
	cubemapDescriptorTextureUpdateInfo.descriptorCount = 1;
	cubemapDescriptorTextureUpdateInfo.textureInfo = lvn::getCubemapTextureData(cubemap);

	std::vector<LvnDescriptorUpdateInfo> cubemapDescriptorUpdateInfo =
	{
		cubemapDescriptorUniformUpdateInfo, cubemapDescriptorTextureUpdateInfo,
	};

	lvn::updateDescriptorLayoutData(cubemapDescriptorLayout, cubemapDescriptorUpdateInfo.data(), cubemapDescriptorUpdateInfo.size());


	// mesh
	LvnMeshCreateInfo meshCreateInfo{};
	meshCreateInfo.bufferInfo.type = Lvn_BufferType_Vertex | Lvn_BufferType_Index;
	meshCreateInfo.bufferInfo.pVertexBindingDescriptions = &lvnVertexBindingDescription;
	meshCreateInfo.bufferInfo.vertexBindingDescriptionCount = 1;
	meshCreateInfo.bufferInfo.pVertexAttributes = lvnAttributes;
	meshCreateInfo.bufferInfo.vertexAttributeCount = 6;
	meshCreateInfo.bufferInfo.pVertices = lvnVertices.data();
	meshCreateInfo.bufferInfo.vertexBufferSize = lvnVertices.size() * sizeof(LvnVertex);
	meshCreateInfo.bufferInfo.pIndices = indices;
	meshCreateInfo.bufferInfo.indexBufferSize = sizeof(indices);

	LvnMesh mesh = lvn::createMesh(&meshCreateInfo);

	LvnCameraCreateInfo cameraCreateInfo{};
	cameraCreateInfo.width = lvn::getWindowSize(window).width;
	cameraCreateInfo.height = lvn::getWindowSize(window).height;
	cameraCreateInfo.position = LvnVec3(0.0f, 0.0f, 2.0f);
	cameraCreateInfo.orientation = LvnVec3(0.0f, 0.0f, -1.0f);
	cameraCreateInfo.upVector = LvnVec3(0.0f, 1.0f, 0.0f);
	cameraCreateInfo.fovDeg = 60.0f;
	cameraCreateInfo.nearPlane = 0.1f;
	cameraCreateInfo.farPlane = 100.0f;
	LvnCamera camera = lvn::createCamera(&cameraCreateInfo);


	LvnModel lvnmodel = lvn::createModel("/home/bma/Documents/blender/models/key/key.gltf");

	auto startTime = std::chrono::high_resolution_clock::now();

	Timer timer;
	int fps;
	timer.start();

	float oldTime = 0.0f;
	Timer deltaTime;
	deltaTime.start();

	UniformData uniformData{};
	int winWidth, winHeight;

	while (lvn::windowOpen(window))
	{
		lvn::updateWindow(window);

		// auto [x, y] = lvn::getWindowDimensions(window);
		// LVN_TRACE("(x:%d,y:%d)", x, y);
		auto windowSize = lvn::getWindowDimensions(window);
		int width = windowSize.width, height = windowSize.height;

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		lvn::mat4 model = lvn::scale(lvn::mat4(1.0f), lvn::vec3(0.5f)) * lvn::rotate(lvn::mat4(1.0f), time * lvn::radians(30.0f), lvn::vec3(0.0f, 0.0f, 1.0f));

		camera.width = width;
		camera.height = height;

		float timeNow = deltaTime.elapsed();
		float dt = timeNow - oldTime;
		oldTime = timeNow;
		cameraMovment(window, &camera, dt);

		lvn::updateCameraMatrix(&camera);
		
		uniformData.matrix = camera.matrix * model;

		if (winWidth != width || winHeight != height)
		{
			lvn::updateFrameBuffer(frameBuffer, width, height);

			fbDescriptorUniformUpdateInfo.descriptorType = Lvn_DescriptorType_UniformBuffer;
			fbDescriptorUniformUpdateInfo.binding = 0;
			fbDescriptorUniformUpdateInfo.descriptorCount = 1;
			fbDescriptorUniformUpdateInfo.bufferInfo = fbUniformBuffer;

			fbDescriptorTextureUpdateInfo.descriptorType = Lvn_DescriptorType_CombinedImageSampler;
			fbDescriptorTextureUpdateInfo.binding = 1;
			fbDescriptorTextureUpdateInfo.descriptorCount = 1;
			fbDescriptorTextureUpdateInfo.textureInfo = lvn::getFrameBufferImage(frameBuffer, 0);

			fbDescriptorUpdateInfo =
			{
				fbDescriptorUniformUpdateInfo, fbDescriptorTextureUpdateInfo,
			};

			lvn::updateDescriptorLayoutData(fbDescriptorLayout, fbDescriptorUpdateInfo.data(), fbDescriptorUpdateInfo.size());
		}

		lvn::renderBeginNextFrame(window);
		lvn::renderBeginCommandRecording(window);
		lvn::renderCmdBeginFrameBuffer(window, frameBuffer);


		lvn::renderCmdBindPipeline(window, fbPipeline);
		lvn::updateUniformBufferData(window, uniformBuffer, &uniformData, sizeof(UniformData));
		lvn::renderCmdBindDescriptorLayout(window, pipeline, descriptorLayout);

		for (uint32_t i = 0; i < lvnmodel.meshes.size(); i++)
		{
			lvn::renderCmdBindVertexBuffer(window, lvn::getMeshBuffer(&lvnmodel.meshes[i]));
			lvn::renderCmdBindIndexBuffer(window, lvn::getMeshBuffer(&lvnmodel.meshes[i]));

			lvn::renderCmdDrawIndexed(window, lvnmodel.meshes[i].indexCount);
		}

		lvn::renderCmdBindVertexBuffer(window, lvn::getMeshBuffer(&mesh));
		lvn::renderCmdBindIndexBuffer(window, lvn::getMeshBuffer(&mesh));

		lvn::renderCmdDrawIndexed(window, sizeof(indices) / sizeof(indices[0]));

		// draw cubemap
		LvnMat4 projection = camera.projectionMatrix;
		LvnMat4 view = LvnMat4(LvnMat3(camera.viewMatrix));
		uniformData.matrix = projection * view;

		lvn::renderCmdBindPipeline(window, cubemapPipeline);
		lvn::updateUniformBufferData(window, cubemapUniformBuffer, &uniformData, sizeof(UniformData));
		lvn::renderCmdBindDescriptorLayout(window, cubemapPipeline, cubemapDescriptorLayout);

		lvn::renderCmdBindVertexBuffer(window, cubemapBuffer);
		lvn::renderCmdBindIndexBuffer(window, cubemapBuffer);

		lvn::renderCmdDrawIndexed(window, 36);

		lvn::renderCmdEndFrameBuffer(window, frameBuffer);


		// begin main render pass
		lvn::renderClearColor(window, abs(sin(time)) * 0.1f, 0.0f, abs(cos(time)) * 0.1f, 1.0f);
		lvn::renderCmdBeginRenderPass(window);

		uniformData.matrix = lvn::mat4(1.0f);

		lvn::renderCmdBindPipeline(window, pipeline);
		lvn::updateUniformBufferData(window, fbUniformBuffer, &uniformData, sizeof(UniformData));

		lvn::renderCmdBindVertexBuffer(window, buffer);
		lvn::renderCmdBindIndexBuffer(window, buffer);

		lvn::renderCmdBindDescriptorLayout(window, fbPipeline, fbDescriptorLayout);
		lvn::renderCmdDrawIndexed(window, sizeof(indices) / sizeof(indices[0]));

		lvn::renderCmdEndRenderPass(window);
		lvn::renderEndCommandRecording(window);
		lvn::renderDrawSubmit(window);

		winWidth = width;
		winHeight = height;

		fps++;
		if (timer.elapsed() >= 1.0f)
		{
			LVN_TRACE("FPS: %d", fps);
			timer.reset();
			fps = 0;
		}
	}

	lvn::destroyModel(&lvnmodel);
	lvn::destroyMesh(&mesh);
	lvn::destroyCubemap(cubemap);
	lvn::destroyFrameBuffer(frameBuffer);
	lvn::destroyTexture(texture);
	lvn::destroyBuffer(buffer);
	lvn::destroyBuffer(cubemapBuffer);

	lvn::destroyUniformBuffer(cubemapUniformBuffer);
	lvn::destroyUniformBuffer(fbUniformBuffer);
	lvn::destroyUniformBuffer(uniformBuffer);

	lvn::destroyDescriptorLayout(cubemapDescriptorLayout);
	lvn::destroyDescriptorLayout(fbDescriptorLayout);
	lvn::destroyDescriptorLayout(descriptorLayout);

	lvn::destroyPipeline(cubemapPipeline);
	lvn::destroyPipeline(fbPipeline);
	lvn::destroyPipeline(pipeline);

	lvn::destroyWindow(window);
	lvn::terminateContext();

	return 0;
}
