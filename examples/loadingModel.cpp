#include <levikno/levikno.h>

#include <vector>

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


layout(binding = 0) uniform MatrixUniforms
{
    mat4 matrix;
    mat4 model;
} ubo;

void main()
{
    gl_Position = ubo.matrix * vec4(inPos, 1.0);
    fragColor = vec3(inColor);
    fragNormal = mat3(transpose(inverse(ubo.model))) * inNormals;
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
    lvnCreateInfo.logging.enableGraphicsApiDebugLogs = true;
    lvnCreateInfo.windowapi = Lvn_WindowApi_glfw;
    lvnCreateInfo.graphicsapi = Lvn_GraphicsApi_vulkan;

    lvn::createContext(&lvnCreateInfo);


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
        { 0, 0, Lvn_AttributeFormat_Vec3_f32, 0 },                   // pos
        { 0, 1, Lvn_AttributeFormat_Vec4_f32, 3 * sizeof(float) },   // color
        { 0, 2, Lvn_AttributeFormat_Vec2_f32, 7 * sizeof(float) },   // texUV
        { 0, 3, Lvn_AttributeFormat_Vec3_f32, 9 * sizeof(float) },   // normal
        { 0, 4, Lvn_AttributeFormat_Vec3_f32, 12 * sizeof(float) },  // tangent
        { 0, 5, Lvn_AttributeFormat_Vec3_f32, 15 * sizeof(float) },  // bitangent
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
    descriptorStorageBinding.descriptorType = Lvn_DescriptorType_UniformBuffer;
    descriptorStorageBinding.shaderStage = Lvn_ShaderStage_Vertex;
    descriptorStorageBinding.descriptorCount = 1;
    descriptorStorageBinding.maxAllocations = MAX_OBJECTS;

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
    descriptorLayoutCreateInfo.maxSets = MAX_OBJECTS;

    // create descriptor layout
    LvnDescriptorLayout* descriptorLayout;
    lvn::createDescriptorLayout(&descriptorLayout, &descriptorLayoutCreateInfo);


    // get the render pass from the window to pass into the pipeline
    LvnRenderPass* renderPass = lvn::windowGetRenderPass(window);

    // create pipeline specification or fixed functions
    LvnPipelineSpecification pipelineSpec = lvn::configPipelineSpecificationInit();
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
    pipelineCreateInfo.renderPass = renderPass;

    // create pipeline
    LvnPipeline* pipeline;
    lvn::createPipeline(&pipeline, &pipelineCreateInfo);

    // destroy the shader after creating the pipeline
    lvn::destroyShader(shader);


    // [Create uniform buffer]
    // uniform storage create info struct
    LvnBufferCreateInfo matrixUniformBufferCreateInfo{};
    matrixUniformBufferCreateInfo.type = Lvn_BufferType_Uniform;
    matrixUniformBufferCreateInfo.usage = Lvn_BufferUsage_Dynamic;
    matrixUniformBufferCreateInfo.size = sizeof(UniformData) * MAX_OBJECTS;
    matrixUniformBufferCreateInfo.data = nullptr;

    // create storage buffer
    LvnBuffer* matrixUniformBuffer;
    lvn::createBuffer(&matrixUniformBuffer, &matrixUniformBufferCreateInfo);

    // uniform buffer create info struct
    LvnBufferCreateInfo uniformBufferCreateInfo{};
    uniformBufferCreateInfo.type = Lvn_BufferType_Uniform;
    uniformBufferCreateInfo.usage = Lvn_BufferUsage_Dynamic;
    uniformBufferCreateInfo.size = sizeof(UniformLightData);
    uniformBufferCreateInfo.data = nullptr;

    // create uniform buffer
    LvnBuffer* uniformBuffer;
    lvn::createBuffer(&uniformBuffer, &uniformBufferCreateInfo);


    // [Load model]
    LvnModel model = lvn::loadModel("res/models/teapot.gltf");


    // update descriptor set
    uint64_t uniformBufferCount = 0;
    for (uint32_t i = 0; i < model.meshes.size(); i++)
    {
        for (uint32_t j = 0; j < model.meshes[i].primitives.size(); j++)
        {
            LvnUniformBufferInfo bufferInfo{};
            bufferInfo.buffer = matrixUniformBuffer;
            bufferInfo.range = sizeof(UniformData);
            bufferInfo.offset = sizeof(UniformData) * uniformBufferCount;
            uniformBufferCount++;

            LvnUniformBufferInfo descriptorBufferInfo{};
            descriptorBufferInfo.buffer = uniformBuffer;
            descriptorBufferInfo.range = sizeof(UniformLightData);
            descriptorBufferInfo.offset = 0;

            LvnDescriptorUpdateInfo descriptorStorageBufferUpdateInfo{};
            descriptorStorageBufferUpdateInfo.descriptorType = Lvn_DescriptorType_UniformBuffer;
            descriptorStorageBufferUpdateInfo.binding = 0;
            descriptorStorageBufferUpdateInfo.descriptorCount = 1;
            descriptorStorageBufferUpdateInfo.bufferInfo = &bufferInfo;

            LvnDescriptorUpdateInfo descriptorUniformBufferUpdateInfo{};
            descriptorUniformBufferUpdateInfo.descriptorType = Lvn_DescriptorType_UniformBuffer;
            descriptorUniformBufferUpdateInfo.binding = 1;
            descriptorUniformBufferUpdateInfo.descriptorCount = 1;
            descriptorUniformBufferUpdateInfo.bufferInfo = &descriptorBufferInfo;

            LvnDescriptorUpdateInfo descriptorUpdateInfo[] =
            {
                descriptorStorageBufferUpdateInfo, descriptorUniformBufferUpdateInfo,
            };

            lvn::allocateDescriptorSet(&model.meshes[i].primitives[j].descriptorSet, descriptorLayout);
            lvn::updateDescriptorSetData(model.meshes[i].primitives[j].descriptorSet, descriptorUpdateInfo, ARRAY_LEN(descriptorUpdateInfo));
        }
    }

    std::vector<UniformData> uniformData; uniformData.resize(uniformBufferCount);

    UniformLightData lightData{};

    // [Main Render Loop]
    while (lvn::windowOpen(window))
    {
        lvn::windowUpdate(window);
        lvn::windowPollEvents();

        int width, height;
        lvn::windowGetSize(window, &width, &height);

        LvnVec3 camPos = { 3.0f, 3.0f, -3.0f };

        // update matrix
        LvnMat4 proj = lvn::perspective(lvn::radians(60.0f), (float)width / (float)height, 0.01f, 1000.0f);
        LvnMat4 view = lvn::lookAt(camPos, LvnVec3(0.0f, 0.0f, 0.0f), LvnVec3(0.0f, 1.0f, 0.0f));
        LvnMat4 modelMatrix = lvn::rotate(LvnMat4(1.0f), lvn::radians(lvn::getContextTime() * 50.0f), LvnVec3(0.0f, 1.0f, 0.0f));
        LvnMat4 camera = proj * view * modelMatrix;

        for (uint32_t i = 0; i < model.meshes.size(); i++)
        {
            // NOTE: don't do this if the model has a node based structure, otherwise the matrices may not be calculated correctly
            const LvnNode& node = model.nodes[i];
            LvnMat4 nodeMatrix = lvn::translate(LvnMat4(1.0f), node.transform.translation) * lvn::quatToMat4(node.transform.rotation) * lvn::scale(LvnMat4(1.0f), node.transform.scale) * node.matrix;
            uniformData[i].matrix = camera * nodeMatrix;
            uniformData[i].model = modelMatrix * nodeMatrix;
        }

        lvn::bufferUpdateData(matrixUniformBuffer, uniformData.data(), sizeof(UniformData) * uniformData.size(), 0);

        lightData.camPos = camPos;
        lightData.crntPos = { 0.0f, 0.0f, 0.0f };
        lightData.lightPos = { 5.0f, 5.0f, 0.0f };
        lightData.lightColor = { 1.0f, 1.0f, 1.0f };
        lightData.intensity = 10.0f;
        lightData.specular = 0.4f;

        lvn::bufferUpdateData(uniformBuffer, &lightData, sizeof(UniformLightData), 0);

        // get next window swapchain image
        lvn::renderBeginNextFrame(window);
        lvn::renderBeginCommandRecording(window);

        // set background color and begin render pass
        lvn::renderCmdBeginRenderPass(window, 0.1f, 0.1f, 0.1f, 1.0f);

        // bind pipeline
        lvn::renderCmdBindPipeline(window, pipeline);

        // bind model vertex and index buffer
        for (uint32_t i = 0; i < model.meshes.size(); i++)
        {
            for (uint32_t j = 0; j < model.meshes[i].primitives.size(); j++)
            {
                LvnPrimitive& primitive = model.meshes[i].primitives[j];

                lvn::renderCmdBindDescriptorSets(window, pipeline, 0, 1, &primitive.descriptorSet);

                lvn::renderCmdBindVertexBuffer(window, 0, 1, &primitive.buffer, 0);
                lvn::renderCmdBindIndexBuffer(window, primitive.buffer, primitive.indexOffset);

                lvn::renderCmdDrawIndexed(window, primitive.indexCount);
            }
        }

        // end render pass and submit rendering
        lvn::renderCmdEndRenderPass(window);
        lvn::renderEndCommandRecording(window);
        lvn::renderDrawSubmit(window); // note that this function is where we actually submit our render data to the GPU
    }

    // destroy objects after they are finished being used
    lvn::unloadModel(&model);
    lvn::destroyBuffer(uniformBuffer);
    lvn::destroyBuffer(matrixUniformBuffer);
    lvn::destroyPipeline(pipeline);
    lvn::destroyWindow(window);
    lvn::destroyDescriptorLayout(descriptorLayout);

    // terminate the context at the end of the program
    lvn::terminateContext();

    return 0;
}
