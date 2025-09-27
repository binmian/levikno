#include <levikno/lvn_graphics.h>

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


struct UniformData
{
    LvnMat4 matrix;
};


int main(int argc, char** argv)
{
    // [Create Context]
    // create the context to load the library

    lvn::initContext();

    LvnGraphicsContextCreateInfo graphicsInfo{};
    graphicsInfo.windowapi = Lvn_WindowApi_Glfw;
    graphicsInfo.graphicsapi = Lvn_GraphicsApi_vulkan;
    graphicsInfo.enableGraphicsApiDebugLogs = true;

    lvn::initGraphicsContext(&graphicsInfo);

    // window create info struct
    LvnWindowCreateInfo windowInfo = lvn::configWindowInit("simpleMatrix", 800, 600);

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
    bufferCreateInfo.type = Lvn_BufferType_Vertex;
    bufferCreateInfo.usage = Lvn_BufferUsage_Static;
    bufferCreateInfo.data = s_Vertices;
    bufferCreateInfo.size = sizeof(s_Vertices);

    // create buffer
    LvnBuffer* vertexBuffer;
    lvn::createBuffer(&vertexBuffer, &bufferCreateInfo);

    // index buffer create info struct
    bufferCreateInfo.type = Lvn_BufferType_Index;
    bufferCreateInfo.usage = Lvn_BufferUsage_Static;
    bufferCreateInfo.data = s_Indices;
    bufferCreateInfo.size = sizeof(s_Indices);

    // create buffer
    LvnBuffer* indexBuffer;
    lvn::createBuffer(&indexBuffer, &bufferCreateInfo);


    // [Create Pipeline]
    // create the pipeline for how we want to render our scene

    // shader create info struct
    LvnShaderCreateInfo shaderCreateInfo{};
    shaderCreateInfo.vertexSrc = "res/shaders/simpleMatrixVert.spv";
    shaderCreateInfo.fragmentSrc = "res/shaders/simpleMatrixFrag.spv";

    // create shader from source
    LvnShader* shader;
    lvn::createShaderFromFileBin(&shader, &shaderCreateInfo);

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
    LvnPipelineFixedFunctions pipelineFixedFuncs = lvn::configPipelineFixedFuncInit();

    // pipeline create info struct
    LvnPipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.pipelineFixedFuncs = &pipelineFixedFuncs;
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
    LvnBufferCreateInfo uniformBufferCreateInfo{};
    uniformBufferCreateInfo.type = Lvn_BufferType_Uniform;
    uniformBufferCreateInfo.usage = Lvn_BufferUsage_Dynamic;
    uniformBufferCreateInfo.size = sizeof(UniformData);
    uniformBufferCreateInfo.data = nullptr;

    // create uniform buffer
    LvnBuffer* uniformBuffer;
    lvn::createBuffer(&uniformBuffer, &uniformBufferCreateInfo);


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

    LvnCommandPool* cmdPool;
    lvn::createCommandPool(&cmdPool);

    LvnCommandBuffer* cmdBuffer;
    lvn::allocateCommandBuffers(cmdPool, &cmdBuffer, 1);

    // [Main Render Loop]
    while (lvn::windowOpen(window))
    {
        lvn::windowUpdate(window);
        lvn::windowPollEvents();

        int width, height;
        lvn::windowGetSize(window, &width, &height);

        // update matrix
        LvnMat4 proj = lvn::perspective(lvn::radians(60.0f), (float)width / (float)height, 0.01f, 1000.0f);
        LvnMat4 view = lvn::lookAt(LvnVec3(0.0f, 0.0f, -2.0f), LvnVec3(0.0f, 0.0f, 0.0f), LvnVec3(0.0f, 1.0f, 0.0f));
        LvnMat4 camera = proj * view;

        uniformData.matrix = camera;
        lvn::bufferUpdateData(uniformBuffer, &uniformData, sizeof(UniformData), 0);

        // get next window swapchain image
        lvn::renderBeginNextFrame(window, cmdBuffer);
        lvn::renderBeginCommandRecording(cmdBuffer);

        // set background color and begin render pass
        lvn::renderCmdBeginRenderPass(cmdBuffer, window, 0.0f, 0.0f, 0.0f, 1.0f);

        // bind pipeline
        lvn::renderCmdBindPipeline(cmdBuffer, pipeline);

        // bind descriptor set
        lvn::renderCmdBindDescriptorSets(cmdBuffer, pipeline, 0, 1, &descriptorSet);

        // bind vertex and index buffer
        lvn::renderCmdBindVertexBuffer(cmdBuffer, 0, 1, &vertexBuffer, 0);
        lvn::renderCmdBindIndexBuffer(cmdBuffer, indexBuffer, 0);

        // draw sqaure
        lvn::renderCmdDrawIndexed(cmdBuffer, ARRAY_LEN(s_Indices)); // number of elements in indices array (6)

        // end render pass and submit rendering
        lvn::renderCmdEndRenderPass(cmdBuffer);
        lvn::renderEndCommandRecording(cmdBuffer);
        lvn::renderDrawSubmit(window, cmdBuffer); // note that this function is where we actually submit our render data to the GPU
    }

    // destroy objects after they are finished being used
    lvn::destroyBuffer(vertexBuffer);
    lvn::destroyBuffer(indexBuffer);
    lvn::destroyBuffer(uniformBuffer);
    lvn::destroyPipeline(pipeline);
    lvn::destroyDescriptorLayout(descriptorLayout);
    lvn::destroyCommandPool(cmdPool);
    lvn::destroyWindow(window);

    // terminate the contexts at the end of the program
    lvn::terminateGraphicsContext();
    lvn::terminateContext();

    return 0;
}
