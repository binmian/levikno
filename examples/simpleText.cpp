#include <levikno/levikno.h>

#include <sstream>


#define MAX_VERTEX_COUNT (5000)
#define MAX_INDEX_COUNT (5000)
#define LVN_CONFIG_GLYPH(ux0,uy0,ux1,uy1,bx,by,u,a) {{ux0/128.0f,uy0/128.0f,ux1/128.0f,uy1/128.0f},{ux1-ux0,uy1-uy0},{bx,by},u,a}


#define ARRAY_LEN(x) (sizeof(x) / sizeof(x[0]))


static const char* s_VertexShaderSrc = R"(
#version 460

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

layout (binding = 0) uniform ObjectBuffer
{
    mat4 matrix;
} ubo;

void main()
{
    gl_Position = ubo.matrix * vec4(inPos, 0.0, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
}
)";

static const char* s_FragmentShaderSrc = R"(
#version 460

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(binding = 1) uniform sampler2D inTexture;

void main()
{
    float text = texture(inTexture, fragTexCoord).r;
    outColor = vec4(vec3(text) * fragColor, 1.0);
}
)";

struct Vertex
{
    LvnVec2 pos;
    LvnVec3 color;
    LvnVec2 texUV;
};

struct UniformData
{
    LvnMat4 matrix;
};


void drawText(LvnDrawList* list, LvnFont& font, const char* text, LvnVec2 pos, LvnVec3 color, float scale)
{
    for (uint32_t i = 0; i < strlen(text);)
    {
        uint32_t codePointBytes = 0;
        uint32_t codepoint = lvn::decodeCodepointUTF8(&text[i], &codePointBytes);
        LvnFontGlyph glyph = lvn::fontGetGlyph(font, codepoint);
        i += codePointBytes;

        float xpos = pos.x + glyph.bearing.x * scale;
        float ypos = pos.y - (glyph.size.y - glyph.bearing.y) * scale;

        float w = glyph.size.x * scale;
        float h = glyph.size.y * scale;

        Vertex rectVertices[] =
        {
            /*         pos         | color |         texUVs            */
            { {xpos, ypos + h},      color, {glyph.uv.x0, glyph.uv.y0} },
            { {xpos, ypos},          color, {glyph.uv.x0, glyph.uv.y1} },
            { {xpos + w, ypos + h},  color, {glyph.uv.x1, glyph.uv.y0} },
            { {xpos + w, ypos},      color, {glyph.uv.x1, glyph.uv.y1} },
        };

        uint32_t rectIndices[] =
        {
            0, 1, 2,
            2, 1, 3,
        };

        LvnDrawCommand drawCmd{};
        drawCmd.pVertices = rectVertices;
        drawCmd.vertexCount = 4;
        drawCmd.pIndices = rectIndices;
        drawCmd.indexCount = 6;
        drawCmd.vertexStride = sizeof(Vertex);

        list->push_back(drawCmd);

        pos.x += glyph.advance * scale;
    }
}

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
    windowInfo.title = "simpleText";
    windowInfo.width = 1280;
    windowInfo.height = 720;
    windowInfo.minWidth = 800;
    windowInfo.minHeight = 600;

    LvnWindow* window;
    lvn::createWindow(&window, &windowInfo);


    // [Create Buffer]
    // create the buffer to store our vertex data

    // create the vertex attributes and descriptor bindings to layout our vertex data
    LvnVertexAttribute attributes[] = 
    {
        { 0, 0, Lvn_AttributeFormat_Vec2_f32, 0 },
        { 0, 1, Lvn_AttributeFormat_Vec3_f32, (2 * sizeof(float)) },
        { 0, 2, Lvn_AttributeFormat_Vec2_f32, (5 * sizeof(float)) },
    };

    LvnVertexBindingDescription vertexBindingDescription{};
    vertexBindingDescription.binding = 0;
    vertexBindingDescription.stride = sizeof(Vertex);

    // vertex buffer create info struct
    LvnBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.type = Lvn_BufferType_Vertex;
    bufferCreateInfo.usage = Lvn_BufferUsage_Dynamic;
    bufferCreateInfo.data = nullptr;
    bufferCreateInfo.size = MAX_VERTEX_COUNT * sizeof(Vertex);

    // create buffer
    LvnBuffer* vertexBuffer;
    lvn::createBuffer(&vertexBuffer, &bufferCreateInfo);

    // index buffer create info struct
    bufferCreateInfo.type = Lvn_BufferType_Index;
    bufferCreateInfo.usage = Lvn_BufferUsage_Dynamic;
    bufferCreateInfo.data = nullptr;
    bufferCreateInfo.size = MAX_INDEX_COUNT * sizeof(uint32_t);

    // create buffer
    LvnBuffer* indexBuffer;
    lvn::createBuffer(&indexBuffer, &bufferCreateInfo);


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

    LvnDescriptorBinding descriptorTextureBinding{};
    descriptorTextureBinding.binding = 1;
    descriptorTextureBinding.descriptorType = Lvn_DescriptorType_ImageSampler;
    descriptorTextureBinding.shaderStage = Lvn_ShaderStage_Fragment;
    descriptorTextureBinding.descriptorCount = 1;
    descriptorTextureBinding.maxAllocations = 1;

    LvnDescriptorBinding descriptorBindings[] =
    {
        descriptorUniformBinding, descriptorTextureBinding,
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
    LvnBufferCreateInfo uniformBufferCreateInfo{};
    uniformBufferCreateInfo.type = Lvn_BufferType_Uniform;
    uniformBufferCreateInfo.usage = Lvn_BufferUsage_Dynamic;
    uniformBufferCreateInfo.size = sizeof(UniformData);
    uniformBufferCreateInfo.data = nullptr;

    // create uniform buffer
    LvnBuffer* uniformBuffer;
    lvn::createBuffer(&uniformBuffer, &uniformBufferCreateInfo);


    // [Create texture]
    auto codepoints = lvn::getDefaultSupportedCodepoints();
    LvnFont font = lvn::loadFontFromFileTTF("res/fonts/JetBrainsMonoNerdFont-Regular.ttf", 16, codepoints.data(), codepoints.size());

    // texture create info struct
    LvnTextureCreateInfo textureCreateInfo{};
    textureCreateInfo.imageData = font.atlas;
    textureCreateInfo.format = Lvn_TextureFormat_Unorm;
    textureCreateInfo.wrapS = Lvn_TextureMode_Repeat;
    textureCreateInfo.wrapT = Lvn_TextureMode_Repeat;
    textureCreateInfo.minFilter = Lvn_TextureFilter_Linear;
    textureCreateInfo.magFilter = Lvn_TextureFilter_Linear;

    LvnTexture* texture;
    lvn::createTexture(&texture, &textureCreateInfo);

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

    UniformData uniformData{};
    LvnDrawList list{};

    LvnTimer timer;
    timer.begin();

    int progress = 0;

    std::string ascii;
    for (int i = 32; i <= 126; i++)
    {
        ascii += (char)i;
    }

    // [Main Render Loop]
    while (lvn::windowOpen(window))
    {
        lvn::windowUpdate(window);
        lvn::windowPollEvents();

        int width, height;
        lvn::windowGetSize(window, &width, &height);

        // update matrix
        LvnMat4 proj = lvn::ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);
        LvnMat4 view = LvnMat4(1.0f);
        LvnMat4 camera = proj * view;

        uniformData.matrix = camera;
        lvn::bufferUpdateData(uniformBuffer, &uniformData, sizeof(UniformData), 0);


        // text examples
        drawText(&list, font, "hello world", {20.0f, height - 40.0f}, {1.0f, 1.0f, 1.0f}, 1.0f);
        drawText(&list, font, "The quick brown fox jumps over the lazy dog", {20.0f, height - 80.0f}, {1.0f, 0.0f, 1.0f}, 1.0f);
        drawText(&list, font, "this text changes color", {20.0f, height - 120.0f}, {0.0f, abs(sin((float)timer.elapsed())), abs(cos((float)timer.elapsed()))}, 1.0f);
        drawText(&list, font, "<- this text moves ->", {120.0f + sin((float)timer.elapsed() * 2.0f) * 100.0f, height - 160.0f}, {0.0f, 1.0f, 1.0f}, 1.0f);
        drawText(&list, font, "this text changes size (keep in mind the pixel quality when scaling)", {20.0f, height - 200.0f}, {1.0f, 1.0f, 0.0f}, abs(sin(timer.elapsed() * 0.5f)) * 0.5f + 0.5f);

        // progress bar text
        std::string str = "[";
        for (int i = 1; i <= 100; i++)
        {
            str += (i <= progress ? "#" : ".");
        }
        str += "] ";
        str += std::to_string(progress);
        str += "%";

        progress = ceil(abs(sin(timer.elapsed() * 0.1f) * 100.0f));

        drawText(&list, font, str.c_str(), {20.0f, height - 240.0f}, {1.0f, 0.3f, 0.0f}, 0.8f);
        str.clear();

        // window dimension text
        std::ostringstream ss;
        ss << "width: " << width << ", height: " << height;
        drawText(&list, font, ss.str().c_str(), {20.0f, height - 280.0f}, {0.1f, 1.0f, 0.1f}, 1.0f);

        // mouse pos text
        float xpos, ypos;
        lvn::mouseGetPos(window, &xpos, &ypos);
        ss.str(""); ss.clear();
        ss << "mouse pos: (x:" << xpos << ",y:" << ypos << ")";
        drawText(&list, font, ss.str().c_str(), {20.0f, height - 320.0f}, {0.1f, 1.0f, 0.1f}, 1.0f);

        // time text
        std::string time = std::to_string(timer.elapsed()) + " sec";
        drawText(&list, font, time.c_str(), {20.0f, height - 360.0f}, {0.1f, 1.0f, 0.1f}, 1.0f);

        // ascii
        drawText(&list, font, ascii.c_str(), {20.0f, height - 400.0f}, {1.0f, 1.0f, 1.0f}, 1.0f);

        // update buffers
        lvn::bufferUpdateData(vertexBuffer, list.vertices(), list.vertex_size(), 0);
        lvn::bufferUpdateData(indexBuffer, list.indices(), list.index_size(), 0);

        // get next window swapchain image
        lvn::renderBeginNextFrame(window);
        lvn::renderBeginCommandRecording(window);

        // set background color and begin render pass
        lvn::renderCmdBeginRenderPass(window, 0.0f, 0.0f, 0.0f, 1.0f);

        // bind pipeline
        lvn::renderCmdBindPipeline(window, pipeline);

        // bind descriptor set
        lvn::renderCmdBindDescriptorSets(window, pipeline, 0, 1, &descriptorSet);

        // bind vertex and index buffer
        lvn::renderCmdBindVertexBuffer(window, 0, 1, &vertexBuffer, 0);
        lvn::renderCmdBindIndexBuffer(window, indexBuffer, 0);

        lvn::renderCmdDrawIndexed(window, list.index_count());

        // end render pass and submit rendering
        lvn::renderCmdEndRenderPass(window);
        lvn::renderEndCommandRecording(window);
        lvn::renderDrawSubmit(window); // note that this function is where we actually submit our render data to the GPU

        list.clear();
    }

    // destroy objects after they are finished being used
    lvn::destroyTexture(texture);
    lvn::destroyBuffer(vertexBuffer);
    lvn::destroyBuffer(indexBuffer);
    lvn::destroyBuffer(uniformBuffer);
    lvn::destroyPipeline(pipeline);
    lvn::destroyDescriptorLayout(descriptorLayout);
    lvn::destroyWindow(window);

    // terminate the context at the end of the program
    lvn::terminateContext();

    return 0;
}
