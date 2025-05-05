#include <levikno/levikno.h>


#define ARRAY_LEN(x) (sizeof(x) / sizeof(x[0]))

#define MAX_VERTEX_COUNT (5000)
#define MAX_INDEX_COUNT (5000)

struct Vertex
{
    LvnVec2 pos;
    LvnVec3 color;
    LvnVec2 texUVs;
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


struct UniformData
{
    LvnMat4 matrix;
};

struct Box
{
    LvnVec2 pos, size;
    LvnVec3 color;
};

struct Ball
{
    LvnVec2 pos, size;
    LvnVec3 color;

    float velocity, angle;
};

static const char* s_VertexShaderSrc = R"(
#version 460

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;


layout (binding = 0) uniform ObjectBuffer
{
    mat4 matrix;
} ubo;


void main()
{
    gl_Position = ubo.matrix * vec4(inPos, 0.0, 1.0);
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

static const char* s_FontVertexShaderSrc = R"(
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

static const char* s_FontFragmentShaderSrc = R"(
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

void drawRect(LvnDrawList* list, LvnVec2 pos, LvnVec2 size, LvnVec3 color)
{
    Vertex vertices[] =
    {
        {{ pos.x, pos.y + size.y },          color, {0.0f, 1.0f} },
        {{ pos.x, pos.y },                   color, {0.0f, 0.0f} },
        {{ pos.x + size.x, pos.y },          color, {1.0f, 0.0f} },
        {{ pos.x + size.x, pos.y + size.y }, color, {1.0f, 1.0f} },
    };

    uint32_t indices[] = { 0, 1, 2, 0, 2, 3 };

    LvnDrawCommand drawCmd{};
    drawCmd.pVertices = vertices;
    drawCmd.vertexCount = 4;
    drawCmd.pIndices = indices;
    drawCmd.indexCount = 6;
    drawCmd.vertexStride = sizeof(Vertex);

    list->push_back(drawCmd);
}

void drawText(LvnDrawList* list, LvnFont& font, const char* text, LvnVec2 pos, LvnVec3 color, float scale)
{
    for (uint32_t i = 0; i < strlen(text); i++)
    {
        LvnFontGlyph glyph = lvn::fontGetGlyph(font, text[i]);

        float xpos = pos.x + glyph.bearing.x * scale;
        float ypos = pos.y + glyph.bearing.y * scale;

        float w = glyph.size.x * scale;
        float h = glyph.size.y * scale;

        Vertex rectVertices[] =
        {
            /*         pos         | color |         texUVs            */
            { {xpos, ypos - h},      color, {glyph.uv.x0, glyph.uv.y1} },
            { {xpos, ypos},          color, {glyph.uv.x0, glyph.uv.y0} },
            { {xpos + w, ypos - h},  color, {glyph.uv.x1, glyph.uv.y1} },
            { {xpos + w, ypos},      color, {glyph.uv.x1, glyph.uv.y0} },
        };

        uint32_t rectIndices[] =
        {
            0, 1, 2,
            2, 1, 3,
        };

        LvnDrawCommand drawCmd{};
        drawCmd.pVertices = (float*)rectVertices;
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
    // create contex and render init
    LvnContextCreateInfo lvnCreateInfo{};
    lvnCreateInfo.logging.enableLogging = true;
    lvnCreateInfo.logging.enableGraphicsApiDebugLogs = true;
    lvnCreateInfo.windowapi = Lvn_WindowApi_glfw;
    lvnCreateInfo.graphicsapi = Lvn_GraphicsApi_vulkan;

    lvn::createContext(&lvnCreateInfo);


    // create window
    LvnWindowCreateInfo windowInfo{};
    windowInfo.title = "Pong";
    windowInfo.width = 800;
    windowInfo.height = 600;
    windowInfo.minWidth = 800;
    windowInfo.minHeight = 600;

    LvnWindow* window;
    lvn::createWindow(&window, &windowInfo);


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

    // create vertex buffer
    LvnBuffer* vertexBuffer;
    lvn::createBuffer(&vertexBuffer, &bufferCreateInfo);
    LvnBuffer* fontVertexBuffer;
    lvn::createBuffer(&fontVertexBuffer, &bufferCreateInfo);

    // index buffer create info struct
    bufferCreateInfo.type = Lvn_BufferType_Index;
    bufferCreateInfo.usage = Lvn_BufferUsage_Dynamic;
    bufferCreateInfo.data = nullptr;
    bufferCreateInfo.size = MAX_INDEX_COUNT * sizeof(uint32_t);

    // create index buffer
    LvnBuffer* indexBuffer;
    lvn::createBuffer(&indexBuffer, &bufferCreateInfo);
    LvnBuffer* fontIndexBuffer;
    lvn::createBuffer(&fontIndexBuffer, &bufferCreateInfo);


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
    descriptorBinding.descriptorType = Lvn_DescriptorType_UniformBuffer;
    descriptorBinding.shaderStage = Lvn_ShaderStage_Vertex;
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
    lvn::allocateDescriptorSet(&descriptorSet, descriptorLayout);

    // get the render pass from the window to pass into the pipeline
    LvnRenderPass* renderPass = lvn::windowGetRenderPass(window);

    // create pipeline specification or fixed functions
    LvnPipelineSpecification pipelineSpec = lvn::configPipelineSpecificationInit();

    // pipeline create info struct
    LvnPipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.pipelineSpecification = &pipelineSpec;
    pipelineCreateInfo.pVertexBindingDescriptions = &vertexBindingDescription;
    pipelineCreateInfo.vertexBindingDescriptionCount = 1;
    pipelineCreateInfo.pVertexAttributes = attributes;
    pipelineCreateInfo.vertexAttributeCount = ARRAY_LEN(attributes);
    pipelineCreateInfo.pDescriptorLayouts = &descriptorLayout;
    pipelineCreateInfo.descriptorLayoutCount = 1;
    pipelineCreateInfo.shader = shader;
    pipelineCreateInfo.renderPass = renderPass;

    // create pipeline
    LvnPipeline* pipeline;
    lvn::createPipeline(&pipeline, &pipelineCreateInfo);

    // destroy the shader after creating the pipeline
    lvn::destroyShader(shader);

    // create pipeline for font rendering
    shaderCreateInfo.vertexSrc = s_FontVertexShaderSrc;
    shaderCreateInfo.fragmentSrc = s_FontFragmentShaderSrc;

    LvnShader* fontShader;
    lvn::createShaderFromSrc(&fontShader, &shaderCreateInfo);

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
    descriptorLayoutCreateInfo.pDescriptorBindings = descriptorBindings;
    descriptorLayoutCreateInfo.descriptorBindingCount = ARRAY_LEN(descriptorBindings);
    descriptorLayoutCreateInfo.maxSets = 1;

    // create descriptor layout
    LvnDescriptorLayout* fontDescriptorLayout;
    lvn::createDescriptorLayout(&fontDescriptorLayout, &descriptorLayoutCreateInfo);

    // create descriptor set using layout
    LvnDescriptorSet* fontDescriptorSet;
    lvn::allocateDescriptorSet(&fontDescriptorSet, fontDescriptorLayout);

    // font pipeline
    pipelineCreateInfo.pipelineSpecification = &pipelineSpec;
    pipelineCreateInfo.pVertexAttributes = attributes;
    pipelineCreateInfo.vertexAttributeCount = ARRAY_LEN(attributes);
    pipelineCreateInfo.pVertexBindingDescriptions = &vertexBindingDescription;
    pipelineCreateInfo.vertexBindingDescriptionCount = 1;
    pipelineCreateInfo.pDescriptorLayouts = &fontDescriptorLayout;
    pipelineCreateInfo.descriptorLayoutCount = 1;
    pipelineCreateInfo.shader = fontShader;
    pipelineCreateInfo.renderPass = renderPass;

    // create pipeline
    LvnPipeline* fontPipeline;
    lvn::createPipeline(&fontPipeline, &pipelineCreateInfo);

    lvn::destroyShader(fontShader);

    // uniform buffer create info
    LvnUniformBufferCreateInfo uniformBufferInfo{};
    uniformBufferInfo.type = Lvn_BufferType_Uniform;
    uniformBufferInfo.size = sizeof(UniformData);

    // create uniform buffer
    LvnUniformBuffer* uniformBuffer;
    lvn::createUniformBuffer(&uniformBuffer, &uniformBufferInfo);


    // [Create font]
    LvnFont font = lvn::loadFontFromFileTTF("res/fonts/PressStart2P.ttf", 32);

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

    // update descriptor sets
    LvnUniformBufferInfo bufferInfo{};
    bufferInfo.buffer = uniformBuffer;
    bufferInfo.range = sizeof(UniformData);
    bufferInfo.offset = 0;

    LvnDescriptorUpdateInfo descriptorUpdateInfo{};
    descriptorUpdateInfo.descriptorType = Lvn_DescriptorType_UniformBuffer;
    descriptorUpdateInfo.binding = 0;
    descriptorUpdateInfo.descriptorCount = 1;
    descriptorUpdateInfo.bufferInfo = &bufferInfo;

    lvn::updateDescriptorSetData(descriptorSet, &descriptorUpdateInfo, 1);

    LvnDescriptorUpdateInfo descriptorTextureUpdateInfo{};
    descriptorTextureUpdateInfo.descriptorType = Lvn_DescriptorType_ImageSampler;
    descriptorTextureUpdateInfo.binding = 1;
    descriptorTextureUpdateInfo.descriptorCount = 1;
    descriptorTextureUpdateInfo.pTextureInfos = &texture;

    LvnDescriptorUpdateInfo descriptorUpdateInfos[] =
    {
        descriptorUpdateInfo, descriptorTextureUpdateInfo,
    };

    lvn::updateDescriptorSetData(fontDescriptorSet, descriptorUpdateInfos, ARRAY_LEN(descriptorUpdateInfos));

    // sound
    LvnSoundCreateInfo soundCreateInfo = lvn::configSoundInit("res/audio/beep.wav");

    LvnSound* sound;
    lvn::createSound(&sound, &soundCreateInfo);

    LvnSoundCreateInfo soundWinCreateInfo = lvn::configSoundInit("res/audio/winBeep.wav");

    LvnSound* soundWin;
    lvn::createSound(&soundWin, &soundWinCreateInfo);


    // draw list and game objects
    LvnDrawList list{};
    UniformData uniformData{};

    float oldTime = 0.0f;
    Timer deltaTime;
    deltaTime.start();

    int width, height;
    lvn::windowGetSize(window, &width, &height);

    float paddleWidth = 20.0f, paddleHeight = 120.0f;
    float paddleSpeed = 1000.0f;

    Box p1{};
    p1.pos = { width * -0.5f + 20.0f, 0.0f };
    p1.size = { paddleWidth, paddleHeight };
    p1.color = { 1.0f, 1.0f, 1.0f };

    Box p2{};
    p2.pos = { width * 0.5f - p2.size.x - 20.0f, 0.0f };
    p2.size = { paddleWidth, paddleHeight };
    p2.color = { 1.0f, 1.0f, 1.0f };


    float ballSpeed = 500.0f;
    Ball ball{};
    ball.pos = { 0.0f, 0.0f };
    ball.size = { 20.0f, 20.0f };
    ball.color = { 1.0f, 1.0f, 1.0f };
    ball.velocity = ballSpeed;
    ball.angle = 30.0f;

    bool ballSide = false;

    uint32_t lscore = 0, rscore = 0;

    LVN_INFO("Use w and s to move the left paddle up and down. Use the up and down arrow keys to move the right paddle up and down");


    while (lvn::windowOpen(window))
    {
        lvn::windowUpdate(window);
        lvn::windowPollEvents();

        int width, height;
        lvn::windowGetSize(window, &width, &height);

        LvnMat4 proj = lvn::ortho((float)width * -0.5f, (float)width * 0.5f, (float)height * -0.5f, (float)height * 0.5f, -1.0f, 1.0f);
        LvnMat4 view = LvnMat4(1.0f);
        LvnMat4 camera = proj * view;

        uniformData.matrix = camera;
        lvn::updateUniformBufferData(uniformBuffer, &uniformData, sizeof(uniformData), 0);


        float paddleSpeed = height;

        float halfWidth = width * 0.5f;
        float halfHeight = height * 0.5f;

        p1.pos.x = -halfWidth + 20.0f;
        p2.pos.x = halfWidth - p2.size.x - 20.0f;

        float timeNow = deltaTime.elapsed();
        float dt = timeNow - oldTime;
        oldTime = timeNow;

        if (lvn::keyPressed(window, Lvn_KeyCode_W))
        {
            p1.pos.y += paddleSpeed * dt;
        }
        if (lvn::keyPressed(window, Lvn_KeyCode_S))
        {
            p1.pos.y -= paddleSpeed * dt;
        }
        if (lvn::keyPressed(window, Lvn_KeyCode_Up))
        {
            p2.pos.y += paddleSpeed * dt;
        }
        if (lvn::keyPressed(window, Lvn_KeyCode_Down))
        {
            p2.pos.y -= paddleSpeed * dt;
        }

        if (p1.pos.y <= -halfHeight) { p1.pos.y = -halfHeight; }
        else if (p1.pos.y >= halfHeight - paddleHeight) { p1.pos.y = halfHeight - paddleHeight; }
        if (p2.pos.y <= -halfHeight) { p2.pos.y = -halfHeight; }
        else if (p2.pos.y >= halfHeight - paddleHeight) { p2.pos.y = halfHeight - paddleHeight; }


        // ball movement

        // calc next ball pos
        ball.velocity = ballSpeed * dt;
        ball.pos.x += ball.velocity * cos(lvn::radians(ball.angle));
        ball.pos.y += ball.velocity * sin(lvn::radians(ball.angle));

        // hit bottom side
        if (ball.pos.y <= -halfHeight)
        {
            ball.pos.y = -halfHeight;
            ball.angle *= -1;
            lvn::soundPlayStart(sound);
        }

        // hit top side
        if (ball.pos.y >= halfHeight - ball.size.y)
        {
            ball.pos.y = halfHeight - ball.size.y;
            ball.angle *= -1;
            lvn::soundPlayStart(sound);
        }

        // hit left side
        if (ball.pos.x <= -halfWidth - ball.size.x)
        {
            ball.pos = {0.0f, 0.0f};
            ball.angle = (rand() % 61) - 30.0f;
            ballSide = false;
            ballSpeed = 500.0f;
            lvn::soundPlayStart(soundWin);
            rscore++;
        }

        // hit right side
        if (ball.pos.x >= halfWidth)
        {
            ball.pos = {0.0f, 0.0f};
            ball.angle = 180.0f - ((rand() % 61) - 30.0f);
            ballSide = true;
            ballSpeed = 500.0f;
            lvn::soundPlayStart(soundWin);
            lscore++;
        }


        // hit left paddle
        if (ball.pos.x <= p1.pos.x + p1.size.x && ballSide &&
            (ball.pos.y + ball.size.y >= p1.pos.y && ball.pos.y <= p1.pos.y + p1.size.y))
        {
            ball.angle = lvn::clampAngleDeg(180.0f - ball.angle + (rand() % 31) - 15.0f);
            if (ball.angle > 45.0f && ball.angle < 90.0f) { ball.angle = 45.0f; }
            else if (ball.angle < 315.0f && ball.angle > 270.0f) { ball.angle = 315.0f; }

            ballSpeed += 10.0f;
            lvn::clamp(ballSpeed, 500.0f, 1500.0f);
            ballSide = false;
            lvn::soundPlayStart(sound);
        }

        // hit right paddle
        if (ball.pos.x + ball.size.x >= p2.pos.x && !ballSide &&
            (ball.pos.y + ball.size.y >= p2.pos.y && ball.pos.y <= p2.pos.y + p2.size.y))
        {
            ball.angle = lvn::clampAngleDeg(180.0f - ball.angle + (rand() % 31) - 15.0f);
            ball.angle = lvn::clamp(ball.angle, 135.0f, 225.0f);
            ballSpeed += 10.0f;
            lvn::clamp(ballSpeed, 500.0f, 1500.0f);
            ballSide = true;
            lvn::soundPlayStart(sound);
        }


        drawRect(&list, p1.pos, p1.size, p1.color);
        drawRect(&list, p2.pos, p2.size, p2.color);
        drawRect(&list, ball.pos, ball.size, ball.color);


        // middle line
        for (uint32_t i = 0; i < 10; i++)
        {
            float yOff = (float)height / 10.0f;
            drawRect(&list, {-5.0f, -halfHeight + yOff * i + yOff * 0.25f}, {10.0f, yOff * 0.5f}, {1.0f, 1.0f, 1.0f});
        }

        lvn::bufferUpdateData(vertexBuffer, list.vertices(), list.vertex_size(), 0);
        lvn::bufferUpdateData(indexBuffer, list.indices(), list.index_size(), 0);

        // begin render
        lvn::renderBeginNextFrame(window);
        lvn::renderBeginCommandRecording(window);

        // set background color and begin render pass
        lvn::renderCmdBeginRenderPass(window, 0.0f, 0.0f, 0.0f, 1.0f);

        // bind pipeline
        lvn::renderCmdBindPipeline(window, pipeline);
        lvn::renderCmdBindDescriptorSets(window, pipeline, 0, 1, &descriptorSet);

        // bind vertex and index buffer
        lvn::renderCmdBindVertexBuffer(window, 0, 1, &vertexBuffer, 0);
        lvn::renderCmdBindIndexBuffer(window, indexBuffer, 0);

        lvn::renderCmdDrawIndexed(window, list.index_count());

        list.clear();

        // font rendering
        drawText(&list, font, std::to_string(lscore).c_str(), {-220.0f, halfHeight - 100.0f}, {1.0f, 1.0f, 1.0f}, 1.0f);
        drawText(&list, font, std::to_string(rscore).c_str(), {150.0f, halfHeight - 100.0f}, {1.0f, 1.0f, 1.0f}, 1.0f);

        lvn::bufferUpdateData(fontVertexBuffer, list.vertices(), list.vertex_size(), 0);
        lvn::bufferUpdateData(fontIndexBuffer, list.indices(), list.index_size(), 0);


        lvn::renderCmdBindPipeline(window, fontPipeline);
        lvn::renderCmdBindDescriptorSets(window, fontPipeline, 0, 1, &fontDescriptorSet);

        lvn::renderCmdBindVertexBuffer(window, 0, 1, &fontVertexBuffer, 0);
        lvn::renderCmdBindIndexBuffer(window, fontIndexBuffer, 0);

        lvn::renderCmdDrawIndexed(window, list.index_count());


        // end render pass and submit rendering
        lvn::renderCmdEndRenderPass(window);
        lvn::renderEndCommandRecording(window);
        lvn::renderDrawSubmit(window); // note that this function is where we actually submit our render data to the GPU

        list.clear();
    }

    lvn::destroySound(sound);
    lvn::destroySound(soundWin);
    lvn::destroyTexture(texture);
    lvn::destroyBuffer(vertexBuffer);
    lvn::destroyBuffer(indexBuffer);
    lvn::destroyBuffer(fontVertexBuffer);
    lvn::destroyBuffer(fontIndexBuffer);
    lvn::destroyPipeline(pipeline);
    lvn::destroyPipeline(fontPipeline);
    lvn::destroyDescriptorLayout(descriptorLayout);
    lvn::destroyDescriptorLayout(fontDescriptorLayout);
    lvn::destroyUniformBuffer(uniformBuffer);
    lvn::destroyWindow(window);

    lvn::terminateContext();

    return 0;
}
