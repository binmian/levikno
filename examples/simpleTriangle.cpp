#include <levikno/lvn_graphics.h>

#define ARRAY_LEN(x) (sizeof(x) / sizeof(x[0]))


static float s_Vertices[] =
{
/*      Pos (x,y,z)   |   color (r,g,b)   */
     0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // v1
    -0.5f,-0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // v2
     0.5f,-0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // v3
};

static uint32_t s_Indices[] = 
{
    0, 1, 2
};

int main(int argc, char** argv)
{
    lvn::initContext();

    LvnGraphicsContextCreateInfo graphicsInfo{};
    graphicsInfo.windowapi = Lvn_WindowApi_Glfw;
    graphicsInfo.graphicsapi = Lvn_GraphicsApi_vulkan;
    graphicsInfo.enableGraphicsApiDebugLogs = true;

    lvn::initGraphicsContext(&graphicsInfo);

    LvnWindowCreateInfo windowInfo = lvn::configWindowInit("simpleTriangle", 800, 600);
    windowInfo.minWidth = 300;
    windowInfo.minHeight = 200;

    LvnWindow* window;
    lvn::createWindow(&window, &windowInfo);

    // [Create Buffer]
    // create the buffer to store our vertex data

    // create the vertex attributes and descriptor bindings to layout our vertex data
    LvnVertexAttribute attributes[2] =
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

    // create vertex buffer
    LvnBuffer* vertexBuffer;
    lvn::createBuffer(&vertexBuffer, &bufferCreateInfo);

    // index buffer create info struct
    bufferCreateInfo.type = Lvn_BufferType_Index;
    bufferCreateInfo.usage = Lvn_BufferUsage_Static;
    bufferCreateInfo.data = s_Indices;
    bufferCreateInfo.size = sizeof(s_Indices);

    // create index buffer
    LvnBuffer* indexBuffer;
    lvn::createBuffer(&indexBuffer, &bufferCreateInfo);


    // [Create Pipeline]
    // create the pipeline for how we want to render our scene

    // shader create info struct
    LvnShaderCreateInfo shaderCreateInfo{};
    shaderCreateInfo.vertexSrc = "/home/bma/Documents/dev/levikno/build/examples/res/shaders/simpleTriangleVert.spv";
    shaderCreateInfo.fragmentSrc = "/home/bma/Documents/dev/levikno/build/examples/res/shaders/simpleTriangleFrag.spv";

    // create shader from source
    LvnShader* shader;
    lvn::createShaderFromFileBin(&shader, &shaderCreateInfo);

    // get the render pass from the window to pass into the pipeline
    LvnRenderPass* renderPass = lvn::windowGetRenderPass(window);

    // create pipeline specification or fixed functions
    LvnPipelineFixedFunctions pipelineFixFuncs = lvn::configPipelineFixedFuncInit();

    // pipeline create info struct
    LvnPipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.pipelineFixedFuncs = &pipelineFixFuncs;
    pipelineCreateInfo.pVertexAttributes = attributes;
    pipelineCreateInfo.vertexAttributeCount = 2;
    pipelineCreateInfo.pVertexBindingDescriptions = &vertexBindingDescription;
    pipelineCreateInfo.vertexBindingDescriptionCount = 1;
    pipelineCreateInfo.shader = shader;
    pipelineCreateInfo.renderPass = renderPass;

    // create pipeline
    LvnPipeline* pipeline;
    lvn::createPipeline(&pipeline, &pipelineCreateInfo);

    // destroy the shader after creating the pipeline
    lvn::destroyShader(shader);

    LvnCommandPool* cmdPool;
    lvn::createCommandPool(&cmdPool);

    LvnCommandBuffer* cmdBuffer;
    lvn::allocateCommandBuffers(cmdPool, &cmdBuffer, 1);

    while (lvn::windowOpen(window))
    {
        lvn::windowUpdate(window);
        lvn::windowPollEvents();

        // get next window swapchain image
        lvn::renderBeginNextFrame(window, cmdBuffer);
        lvn::renderBeginCommandRecording(cmdBuffer);

        // set background color and begin render pass
        lvn::renderCmdBeginRenderPass(cmdBuffer, window, 0.0f, 0.0f, 0.0f, 1.0f);

        // bind pipeline
        lvn::renderCmdBindPipeline(cmdBuffer, pipeline);

        // bind vertex and index buffer
        lvn::renderCmdBindVertexBuffer(cmdBuffer, 0, 1, &vertexBuffer, 0);
        lvn::renderCmdBindIndexBuffer(cmdBuffer, indexBuffer, 0);

        // draw triangle
        lvn::renderCmdDrawIndexed(cmdBuffer, ARRAY_LEN(s_Indices)); // number of elements in indices array (3)

        // end render pass and submit rendering
        lvn::renderCmdEndRenderPass(cmdBuffer);
        lvn::renderEndCommandRecording(cmdBuffer);
        lvn::renderDrawSubmit(window, cmdBuffer); // note that this function is where we actually submit our render data to the GPU
    }

    lvn::destroyPipeline(pipeline);
    lvn::destroyBuffer(vertexBuffer);
    lvn::destroyBuffer(indexBuffer);
    lvn::destroyCommandPool(cmdPool);
    lvn::destroyWindow(window);

    lvn::terminateGraphicsContext();
    lvn::terminateContext();
}
