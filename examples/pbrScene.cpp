#include <levikno/levikno.h>

#include <vector>
#include <unordered_map>

#define ARRAY_LEN(x) (sizeof(x) / sizeof(x[0]))

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

static float s_FbVertices_vk[] =
{
    /*    pos (x,y,z)   |      UV   */
    -1.0f, -1.0f, 0.0f,    1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,    0.0f, 0.0f,
     1.0f,  1.0f, 0.0f,    0.0f, 1.0f,

    -1.0f, -1.0f, 0.0f,    1.0f, 0.0f,
     1.0f,  1.0f, 0.0f,    0.0f, 1.0f,
    -1.0f,  1.0f, 0.0f,    1.0f, 1.0f,
};

static float s_FbVertices_ogl[] =
{
    /*    pos (x,y,z)   |      UV   */
     1.0f,  1.0f, 0.0f,    1.0f, 1.0f,
    -1.0f,  1.0f, 0.0f,    0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f,    0.0f, 0.0f,

     1.0f,  1.0f, 0.0f,    1.0f, 1.0f,
    -1.0f, -1.0f, 0.0f,    0.0f, 0.0f,
     1.0f, -1.0f, 0.0f,    1.0f, 0.0f,
};

// pbr shaders

// vertex shader
static const char* s_VertexShaderSrc = R"(
#version 460

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;
layout(location = 4) in vec3 inTangent;
layout(location = 5) in vec3 inBitangent;
layout(location = 6) in vec4 inJointIndices;
layout(location = 7) in vec4 inWeights;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec4 fragColor;
layout(location = 2) out vec2 fragTexCoord;
layout(location = 3) out vec3 fragLightPos;
layout(location = 4) out vec3 fragViewPos;

layout(binding = 1) uniform UniformBuffer
{
    mat4 matrix;
    mat4 model;
} ubo;

layout(binding = 0) uniform ObjectBuffer
{
    vec3 camPos;
    vec3 lightPos;
    float metalic;
    float roughness;
    float ambientOcclusion;
    bool hasSkin;
} pbrUbo;

layout(std430, binding = 2) readonly buffer JointMatrices
{
    mat4 jointMatrices[];
};

void main()
{
    mat3 normalMatrix = transpose(inverse(mat3(ubo.model)));
    vec3 T = normalize(normalMatrix * inTangent);
    vec3 N = normalize(normalMatrix * inNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    mat3 TBN = transpose(mat3(T, B, N));

    mat4 skinMat;
    if (pbrUbo.hasSkin)
    {
        skinMat =
        inWeights.x * jointMatrices[int(inJointIndices.x)] +
        inWeights.y * jointMatrices[int(inJointIndices.y)] +
        inWeights.z * jointMatrices[int(inJointIndices.z)] +
        inWeights.w * jointMatrices[int(inJointIndices.w)];
    }
    else
        skinMat = mat4(1.0);

    fragPos = TBN * vec3(ubo.model * skinMat * vec4(inPos, 1.0));
    fragColor = inColor;
    fragTexCoord = inTexCoord;
    fragLightPos = TBN * pbrUbo.lightPos;
    fragViewPos = TBN * pbrUbo.camPos;

    gl_Position = ubo.matrix * ubo.model * skinMat * vec4(inPos, 1.0);
}
)";

// fragment shader
static const char* s_FragmentShaderSrc = R"(
#version 460

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec4 fragColor;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec3 fragLightPos;
layout(location = 4) in vec3 fragViewPos;


layout(binding = 0) uniform ObjectBuffer
{
    vec3 camPos;
    vec3 lightPos;
    float metalic;
    float roughness;
    float ambientOcclusion;
    bool hasSkin;
} ubo;

layout(binding = 3) uniform sampler2D albedoTex;
layout(binding = 4) uniform sampler2D metalicRoughnessOcclusionTex;
layout(binding = 5) uniform sampler2D normalTex;
layout(binding = 6) uniform sampler2D emissiveTex;

const float PI = 3.14159265;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    denom = max(denom, 0.000001);

    return num / denom;
}

float GeometrySchlickGGX(float NdotX, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotX;
    float denom = NdotX * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main()
{
    vec3 albedo = vec3(texture(albedoTex, fragTexCoord)) * vec3(fragColor);
    float alpha = texture(albedoTex, fragTexCoord).a;
    if (alpha < 0.1)
        discard;

    float metalic = texture(metalicRoughnessOcclusionTex, fragTexCoord).b;
    float roughness = texture(metalicRoughnessOcclusionTex, fragTexCoord).g;
    float ambientOcclusion = ubo.ambientOcclusion;
    vec3 emissive = vec3(texture(emissiveTex, fragTexCoord));

    vec3 normal = texture(normalTex, fragTexCoord).rgb;
    normal = normalize(normal * 2.0 - 1.0);

    vec3 camPos = fragViewPos;
    vec3 lightPos = fragLightPos;
    vec3 lightColors = vec3(1.0);
    float lightStrength = 10.0;

    vec3 N = normalize(normal);
    vec3 V = normalize(camPos - fragPos);

    vec3 L = normalize(lightPos - fragPos);
    vec3 H = normalize(V + L);

    float distance = length(lightPos - fragPos);
    float attenuation = lightStrength / (distance * distance);
    vec3 radiance = lightColors * attenuation;

    vec3 F0 = vec3(0.2);
    F0 = mix(F0, albedo, metalic);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metalic;

    vec3 numerator = NDF * G * F;
    float denominator = max(4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0), 0.0001);
    vec3 specular = numerator / denominator;

    float NdotL = max(dot(N, L), 0.0);
    vec3 BRDF = (kD * albedo / PI + specular) * radiance * NdotL;

    vec3 ambient = vec3(0.1) * albedo;
    vec3 color = BRDF + emissive + ambient;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));

    outColor = vec4(color, alpha);
}
)";


// cubemap shaders
// vertex shader
static const char* s_CubemapVertexShaderSrc = R"(
#version 460

layout (location = 0) in vec3 inPos;

layout (location = 0) out vec3 fragTexCoord;

layout (std140, binding = 6) uniform ObjectBuffer 
{
    mat4 matrix;
    mat4 model;
} ubo;

void main()
{
    vec4 pos = ubo.matrix * vec4(inPos, 1.0);
    gl_Position = pos.xyww;
    fragTexCoord = vec3(inPos.x, inPos.y, inPos.z);
}
)";

// fragment shader
static const char* s_CubemapFragmentShaderSrc = R"(
#version 460

layout (location = 0) out vec4 outColor;

layout (location = 0) in vec3 fragTexCoord;

layout (binding = 1) uniform samplerCube samplerCubeMap;

void main() 
{
    outColor = texture(samplerCubeMap, fragTexCoord);
}
)";


// framebuffer shaders
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
static const char* s_FbragmentShaderSrc = R"(
#version 460

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec2 fragTexCoord;

layout(binding = 1) uniform sampler2D inTexture;

void main()
{
    outColor = texture(inTexture, fragTexCoord);
}
)";


struct EventData
{
    LvnDescriptorSet* fbDescriptorSet;
    LvnFrameBuffer* frameBuffer;
};

struct UniformData
{
    LvnMat4 matrix;
    LvnMat4 model;
};

struct PbrUniformData
{
    LvnVec3 campPos;
    alignas(16) LvnVec3 lightPos;
    float metalic;
    float roughness;
    float ambientOcclusion;
    int hasSkin;
};

struct CameraView
{
    LvnMat4 projectionMatrix;
    LvnMat4 viewMatrix;
    LvnMat4 matrix;

    LvnVec3 position;
    LvnVec3 orientation;
    LvnVec3 upVector;

    LvnCamera data;
};

// global variables, very scary
LvnWindow* window;
LvnFrameBuffer* frameBuffer;
LvnShader* shader;
LvnDescriptorLayout* descriptorLayout;
LvnPipeline* pipeline;
LvnShader* cubemapShader;
LvnDescriptorLayout* cubemapDescriptorLayout;
LvnDescriptorSet* cubemapDescriptorSet;
LvnPipeline* cubemapPipeline;
LvnShader* fbShader;
LvnDescriptorLayout* fbDescriptorLayout;
LvnPipeline* fbPipeline;
LvnBuffer* fbBuffer;
LvnBuffer* cubemapBuffer;
LvnBuffer* matrixUniformBuffer;
LvnBuffer* defaultSkinUniformBuffer;
LvnBuffer* pbrUniformBuffer;
LvnBuffer* cubemapUniformBuffer;
LvnCubemap* cubemap;
LvnModel lvnmodel;


void cameraMovment(LvnWindow* window, CameraView* camera, float dt);
void orbitMovment(LvnWindow* window, CameraView* camera, float dt);
LvnDescriptorBinding textureBinding(uint32_t binding, uint32_t maxAllocations);
bool windowFrameBufferResize(LvnWindowFramebufferResizeEvent* e, void* userData);
bool mouseScroll(LvnMouseScrolledEvent* e, void* pUserData);
void eventsCallbackFn(LvnEvent* e);
void scaleInput(LvnWindow* window, float* scale, float dt);
LvnMat4 getNodeMatrix(const LvnModel& model, const LvnNode& node);
void updateJoints(const LvnModel& model, const LvnNode& node);
void updateAnimation(LvnModel& model, float dt);
void updateNodeDescriptorSets(LvnModel& model, const std::vector<int32_t>& nodes);
void updateNodeMatrix(LvnModel& model, const std::vector<int32_t>& nodes, std::vector<UniformData>& objectData, CameraView camera);
void drawNode(LvnModel& model, const std::vector<int32_t>& nodes);

static float s_CameraSpeed = 5.0f;
static bool s_CameraFirstClick = true;
static float m_LastMouseX = 0.0f, m_LastMouseY = 0.0f;
static float s_CameraSensitivity = 10.0f;

void cameraMovment(LvnWindow* window, CameraView* camera, float dt)
{
    if (lvn::keyPressed(window, Lvn_KeyCode_W))
        camera->position += (s_CameraSpeed * camera->orientation) * dt;

    if (lvn::keyPressed(window, Lvn_KeyCode_A))
        camera->position += (s_CameraSpeed * -lvn::normalize(lvn::cross(camera->orientation, camera->upVector))) * dt;

    if (lvn::keyPressed(window, Lvn_KeyCode_S))
        camera->position += (s_CameraSpeed * -camera->orientation) * dt;

    if (lvn::keyPressed(window, Lvn_KeyCode_D))
        camera->position += (s_CameraSpeed * lvn::normalize(lvn::cross(camera->orientation, camera->upVector))) * dt;

    if (lvn::keyPressed(window, Lvn_KeyCode_Space))
        camera->position += (s_CameraSpeed * camera->upVector) * dt;

    if (lvn::keyPressed(window, Lvn_KeyCode_LeftControl))
        camera->position += (s_CameraSpeed * -camera->upVector) * dt;

    if (lvn::keyPressed(window, Lvn_KeyCode_LeftShift))
        s_CameraSpeed = 10.0f;
    else if (lvn::keyReleased(window, Lvn_KeyCode_LeftShift))
        s_CameraSpeed = 5.0f;

    if (lvn::mouseButtonPressed(window, Lvn_MouseButton_1))
    {
        auto mousePos = lvn::mouseGetPos(window);
        
        if (s_CameraFirstClick)
        {
            m_LastMouseX = mousePos.x;
            m_LastMouseY = mousePos.y;
            s_CameraFirstClick = false;
            lvn::mouseSetInputMode(window, Lvn_MouseInputMode_Disable);
        }

        float xoffset = mousePos.x - m_LastMouseX;
        float yoffset = m_LastMouseY - mousePos.y;
        m_LastMouseX = mousePos.x;
        m_LastMouseY = mousePos.y;
        xoffset *= s_CameraSensitivity * dt;
        yoffset *= s_CameraSensitivity * dt;

        lvn::vec3 newOrientation = lvn::rotate(camera->orientation, lvn::radians(yoffset), lvn::normalize(lvn::cross(camera->orientation, camera->upVector)));
        if (abs(lvn::angle(lvn::normalize(newOrientation), camera->upVector) - lvn::radians(90.0f)) <= lvn::radians(85.0f))
        {
            if (abs(lvn::angle(lvn::normalize(newOrientation), lvn::normalize(camera->upVector)) - lvn::radians(90.0f)) <= lvn::radians(85.0f))
                camera->orientation = newOrientation;
        }

        // Rotates the orientation left and right
        camera->orientation = lvn::normalize(lvn::rotate(camera->orientation, lvn::radians(-xoffset), camera->upVector));


    }
    else if (lvn::mouseButtonReleased(window, Lvn_MouseButton_1))
    {
        s_CameraFirstClick = true;
        lvn::mouseSetInputMode(window, Lvn_MouseInputMode_Normal);
    }

    int width, height;
    lvn::windowGetSize(window, &width, &height);
    camera->projectionMatrix = lvn::perspective(lvn::radians(60.0f), (float)width / (float)height, 0.01f, 1000.0f);
    camera->viewMatrix = lvn::lookAt(camera->position, camera->position + camera->orientation, camera->upVector);
    camera->matrix = camera->projectionMatrix * camera->viewMatrix;
}

static float s_Radius = 5.0f;
static float s_AngleX = 0.0f, s_AngleY = 0.0f;
static float s_PanSpeed = 5.0f, s_MoveShiftSpeed = 5.0f;
static float s_OrbitSensitivity = 40.0f;
static LvnVec3 s_Pos = LvnVec3(0.0f);
static LvnMat4 s_Model = LvnMat4(1.0f);

void orbitMovment(LvnWindow* window, CameraView* camera, float dt)
{
    LvnMat4 view = camera->viewMatrix;

    bool mouse1 = lvn::mouseButtonPressed(window, Lvn_MouseButton_1);
    bool mouse2 = lvn::mouseButtonPressed(window, Lvn_MouseButton_2);

    if (lvn::keyPressed(window, Lvn_KeyCode_LeftShift) && mouse1 && !mouse2)
    {
        auto mousePos = lvn::mouseGetPos(window);
        
        if (s_CameraFirstClick)
        {
            m_LastMouseX = mousePos.x;
            m_LastMouseY = mousePos.y;
            s_CameraFirstClick = false;
            lvn::mouseSetInputMode(window, Lvn_MouseInputMode_Disable);
        }

        float xoffset = mousePos.x - m_LastMouseX;
        float yoffset = mousePos.y - m_LastMouseY;
        m_LastMouseX = mousePos.x;
        m_LastMouseY = mousePos.y;
        xoffset *= s_MoveShiftSpeed * dt;
        yoffset *= s_MoveShiftSpeed * dt;

        if (lvn::keyPressed(window, Lvn_KeyCode_LeftControl))
        {
            xoffset *= 0.1f;
            yoffset *= 0.1f;
        }

        LvnVec3 right = LvnVec3(view[0][0], view[1][0], view[2][0]);
        LvnVec3 up = LvnVec3(view[0][1], view[1][1], view[2][1]);

        s_Model = lvn::translate(LvnMat4(s_Model), right * -xoffset);
        s_Model = lvn::translate(LvnMat4(s_Model), up * -yoffset);
    }
    else if (mouse1 && !mouse2)
    {
        auto mousePos = lvn::mouseGetPos(window);

        if (s_CameraFirstClick)
        {
            m_LastMouseX = mousePos.x;
            m_LastMouseY = mousePos.y;
            s_CameraFirstClick = false;
            lvn::mouseSetInputMode(window, Lvn_MouseInputMode_Disable);
        }

        float xoffset = mousePos.x - m_LastMouseX;
        float yoffset = mousePos.y - m_LastMouseY;
        m_LastMouseX = mousePos.x;
        m_LastMouseY = mousePos.y;
        xoffset *= s_OrbitSensitivity;
        yoffset *= s_OrbitSensitivity;

        s_AngleX -= xoffset * dt;
        s_AngleY += yoffset * dt;
    }

    if (mouse2 && !mouse1)
    {
        auto mousePos = lvn::mouseGetPos(window);
        if (s_CameraFirstClick)
        {
            m_LastMouseY = mousePos.y;
            s_CameraFirstClick = false;
            lvn::mouseSetInputMode(window, Lvn_MouseInputMode_Disable);
        }

        float yoffset = mousePos.y - m_LastMouseY;
        m_LastMouseY = mousePos.y;
        yoffset *= s_PanSpeed * dt;

        if (lvn::keyPressed(window, Lvn_KeyCode_LeftControl))
            yoffset *= 0.1f;

        s_Radius += yoffset;
        s_Radius = lvn::max(s_Radius, 0.0f);
    }

    if (lvn::mouseButtonReleased(window, Lvn_MouseButton_1) && lvn::mouseButtonReleased(window, Lvn_MouseButton_2))
    {
        s_CameraFirstClick = true;
        lvn::mouseSetInputMode(window, Lvn_MouseInputMode_Normal);
    }

    int width, height;
    lvn::windowGetSize(window, &width, &height);
    camera->projectionMatrix = lvn::perspective(lvn::radians(60.0f), (float)width / (float)height, 0.01f, 1000.0f);

    // NOTE: opengl and vulkan have different coord systems (left hand vs right hand) so this needs to be different, I might add something later to combine these two
    LvnGraphicsApi graphicsapi = lvn::getGraphicsApi();
    if (graphicsapi == Lvn_GraphicsApi_vulkan)
    {
        camera->viewMatrix =
              lvn::translate(LvnMat4(1.0f), LvnVec3(0.0f, 0.0f, s_Radius))
            * lvn::rotate(LvnMat4(1.0f), lvn::radians(s_AngleY), LvnVec3(1.0f, 0.0f, 0.0f))
            * lvn::rotate(LvnMat4(1.0f), lvn::radians(s_AngleX), LvnVec3(0.0f, 1.0f, 0.0f))
            * s_Model;
    }
    else if (graphicsapi == Lvn_GraphicsApi_opengl)
    {
        camera->viewMatrix =
              lvn::translate(LvnMat4(1.0f), LvnVec3(0.0f, 0.0f, -s_Radius))
            * lvn::rotate(LvnMat4(1.0f), lvn::radians(-s_AngleY), LvnVec3(1.0f, 0.0f, 0.0f))
            * lvn::rotate(LvnMat4(1.0f), lvn::radians(s_AngleX), LvnVec3(0.0f, 1.0f, 0.0f))
            * s_Model;
    }

    camera->matrix = camera->projectionMatrix * camera->viewMatrix;

    LvnMat4 inverse = lvn::inverse(camera->viewMatrix);
    camera->position = LvnVec3(inverse[3][0], inverse[3][1], inverse[3][2]);
}

LvnDescriptorBinding textureBinding(uint32_t binding, uint32_t maxAllocations)
{
    LvnDescriptorBinding combinedImageDescriptorBinding{};
    combinedImageDescriptorBinding.binding = binding;
    combinedImageDescriptorBinding.descriptorType = Lvn_DescriptorType_ImageSampler;
    combinedImageDescriptorBinding.shaderStage = Lvn_ShaderStage_Fragment;
    combinedImageDescriptorBinding.descriptorCount = 1;
    combinedImageDescriptorBinding.maxAllocations = maxAllocations;

    return combinedImageDescriptorBinding;
}

bool windowFrameBufferResize(LvnWindowFramebufferResizeEvent* e, void* userData)
{
    EventData* data = static_cast<EventData*>(userData);

    lvn::frameBufferResize(data->frameBuffer, e->width, e->height);

    LvnDescriptorUpdateInfo fbDescriptorUpdateInfo;

    LvnTexture* frameBufferImage = lvn::frameBufferGetImage(data->frameBuffer, 0);

    fbDescriptorUpdateInfo.descriptorType = Lvn_DescriptorType_ImageSampler;
    fbDescriptorUpdateInfo.binding = 1;
    fbDescriptorUpdateInfo.descriptorCount = 1;
    fbDescriptorUpdateInfo.pTextureInfos = &frameBufferImage;

    lvn::updateDescriptorSetData(data->fbDescriptorSet, &fbDescriptorUpdateInfo, 1);

    return true;
}

bool mouseScroll(LvnMouseScrolledEvent* e, void* pUserData)
{
    s_Radius += -e->y * 0.5f;
    s_Radius = lvn::max(s_Radius, 0.0f);

    return true;
}

void eventsCallbackFn(LvnEvent* e)
{
    lvn::dispatchWindowFramebufferResizeEvent(e, windowFrameBufferResize);
    lvn::dispatchMouseScrolledEvent(e, mouseScroll);
}

void scaleInput(LvnWindow* window, float* scale, float dt)
{
    if (lvn::keyPressed(window, Lvn_KeyCode_Equal))
    {
        *scale += dt;
    }
    if (lvn::keyPressed(window, Lvn_KeyCode_Minus))
    {
        *scale -= dt;
    }

    if (*scale <= 0.0) *scale = 0.01f;
}

LvnMat4 getNodeMatrix(const LvnModel& model, const LvnNode& node)
{
    LvnMat4 nodeMatrix = lvn::translate(LvnMat4(1.0f), node.transform.translation) * lvn::quatToMat4(node.transform.rotation) * lvn::scale(LvnMat4(1.0f), node.transform.scale) * node.matrix;
    if (node.parent >= 0)
    {
        const LvnNode& parrent = model.nodes[node.parent];
        nodeMatrix = getNodeMatrix(model, parrent) * nodeMatrix;
    }

    return nodeMatrix;
}

static std::vector<LvnMat4> s_JointMatrices;
static std::unordered_map<int, LvnBuffer*> s_CacheSkinSSBO; // keep track of skins that are already updated so that they arent updated again

void updateJoints(const LvnModel& model, const LvnNode& node)
{
    if (node.skin >= 0)
    {
        const LvnSkin& skin = lvnmodel.skins[node.skin];

        if (s_CacheSkinSSBO.find(node.skin) != s_CacheSkinSSBO.end())
            return;
        else
            s_CacheSkinSSBO[node.skin] = skin.ssbo;

        s_JointMatrices.resize(skin.joints.size());
        for (size_t i = 0; i < skin.joints.size(); i++)
        {
            s_JointMatrices[i] = getNodeMatrix(model, model.nodes[skin.joints[i]]) * skin.inverseBindMatrices[i];
        }

        lvn::bufferUpdateData(skin.ssbo, s_JointMatrices.data(), s_JointMatrices.size() * sizeof(LvnMat4), 0);
    }
}

void updateAnimation(LvnModel& model, float dt)
{
    if (model.animations.empty())
        return;

    LvnAnimation& animation = model.animations[0];
    animation.currentTime += dt;

    if (animation.currentTime > animation.end)
        animation.currentTime -= animation.end;

    for (const LvnAnimationChannel& channel : animation.channels)
    {
        for (uint32_t i = 0; i < channel.keyFrames.size(); i++)
        {
            if (channel.interpolation == Lvn_InterpolationMode_Linear)
            {
                if ((animation.currentTime >= channel.keyFrames[i]) && (animation.currentTime <= channel.keyFrames[i + 1]))
                {
                    float a = (animation.currentTime - channel.keyFrames[i]) / (channel.keyFrames[i + 1] - channel.keyFrames[i]);
                    if (channel.path == Lvn_AnimationPath_Translation)
                    {
                        model.nodes[channel.node].transform.translation = lvn::lerp(channel.outputs[i], channel.outputs[i + 1], a);
                        break;
                    }
                    else if (channel.path == Lvn_AnimationPath_Rotation)
                    {
                        LvnQuat q1;
                        q1.x = channel.outputs[i].x;
                        q1.y = channel.outputs[i].y;
                        q1.z = channel.outputs[i].z;
                        q1.w = channel.outputs[i].w;

                        LvnQuat q2;
                        q2.x = channel.outputs[i + 1].x;
                        q2.y = channel.outputs[i + 1].y;
                        q2.z = channel.outputs[i + 1].z;
                        q2.w = channel.outputs[i + 1].w;

                        model.nodes[channel.node].transform.rotation = lvn::normalize(lvn::slerp(q1, q2, a));
                        break;
                    }
                    else if (channel.path == Lvn_AnimationPath_Scale)
                    {
                        model.nodes[channel.node].transform.scale = lvn::lerp(channel.outputs[i], channel.outputs[i + 1], a);
                        break;
                    }
                }
            }
            else if (channel.interpolation == Lvn_InterpolationMode_Step)
            {
                if ((animation.currentTime >= channel.keyFrames[i]) && (animation.currentTime <= channel.keyFrames[i + 1]))
                {
                    if (channel.path == Lvn_AnimationPath_Translation)
                    {
                        model.nodes[channel.node].transform.translation = channel.outputs[i];
                        break;
                    }
                    else if (channel.path == Lvn_AnimationPath_Rotation)
                    {
                        LvnQuat q1;
                        q1.x = channel.outputs[i].x;
                        q1.y = channel.outputs[i].y;
                        q1.z = channel.outputs[i].z;
                        q1.w = channel.outputs[i].w;

                        model.nodes[channel.node].transform.rotation = q1;
                        break;
                    }
                    else if (channel.path == Lvn_AnimationPath_Scale)
                    {
                        model.nodes[channel.node].transform.scale = channel.outputs[i];
                        break;
                    }
                }
            }
        }
    }

    s_CacheSkinSSBO.clear();

    for (auto& skin : model.skins)
        for (auto& node : skin.joints)
            updateJoints(model, model.nodes[node]);
}

uint32_t buffOffset = 0;
void updateNodeDescriptorSets(LvnModel& model, const std::vector<int32_t>& nodes)
{
    LvnUniformBufferInfo descriptorPbrBufferInfo{};
    descriptorPbrBufferInfo.buffer = pbrUniformBuffer;
    descriptorPbrBufferInfo.range = sizeof(PbrUniformData);
    descriptorPbrBufferInfo.offset = 0;

    LvnDescriptorUpdateInfo descriptorPbrUniformUpdateInfo{};
    descriptorPbrUniformUpdateInfo.descriptorType = Lvn_DescriptorType_UniformBuffer;
    descriptorPbrUniformUpdateInfo.binding = 0;
    descriptorPbrUniformUpdateInfo.descriptorCount = 1;
    descriptorPbrUniformUpdateInfo.bufferInfo = &descriptorPbrBufferInfo;

    for (const int32_t& nodeIndex : nodes)
    {
        const LvnNode& node = model.nodes[nodeIndex];
        if (node.mesh >= 0)
        {
            for (auto& primitive : model.meshes[node.mesh].primitives)
            {
                LvnUniformBufferInfo bufferInfo{};
                bufferInfo.buffer = matrixUniformBuffer;
                bufferInfo.range = sizeof(UniformData);
                bufferInfo.offset = sizeof(UniformData) * buffOffset;
                buffOffset++;

                LvnDescriptorUpdateInfo descriptorMatrixUniformUpdateInfo{};
                descriptorMatrixUniformUpdateInfo.descriptorType = Lvn_DescriptorType_UniformBuffer;
                descriptorMatrixUniformUpdateInfo.binding = 1;
                descriptorMatrixUniformUpdateInfo.descriptorCount = 1;
                descriptorMatrixUniformUpdateInfo.bufferInfo = &bufferInfo;

                LvnBuffer* skinBuffer = node.skin >= 0 ? lvnmodel.skins[node.skin].ssbo : defaultSkinUniformBuffer;

                LvnUniformBufferInfo skinBufferInfo{};
                skinBufferInfo.buffer = skinBuffer;
                skinBufferInfo.range = (node.skin >= 0 ? lvnmodel.skins[node.skin].inverseBindMatrices.size() : 1) * sizeof(LvnMat4);
                skinBufferInfo.offset = 0;

                LvnDescriptorUpdateInfo descriptorSkinUniformUpdateInfo{};
                descriptorSkinUniformUpdateInfo.descriptorType = Lvn_DescriptorType_StorageBuffer;
                descriptorSkinUniformUpdateInfo.binding = 2;
                descriptorSkinUniformUpdateInfo.descriptorCount = 1;
                descriptorSkinUniformUpdateInfo.bufferInfo = &skinBufferInfo;

                lvn::allocateDescriptorSet(&primitive.descriptorSet, descriptorLayout);

                LvnDescriptorUpdateInfo pbrDescriptorUpdateInfo[] =
                {
                    descriptorPbrUniformUpdateInfo,
                    descriptorMatrixUniformUpdateInfo,
                    descriptorSkinUniformUpdateInfo,
                    { 3, Lvn_DescriptorType_ImageSampler, 1, nullptr, &primitive.material.albedo },
                    { 4, Lvn_DescriptorType_ImageSampler, 1, nullptr, &primitive.material.metallicRoughnessOcclusion },
                    { 5, Lvn_DescriptorType_ImageSampler, 1, nullptr, &primitive.material.normal },
                    { 6, Lvn_DescriptorType_ImageSampler, 1, nullptr, &primitive.material.emissive },
                };

                lvn::updateDescriptorSetData(primitive.descriptorSet, pbrDescriptorUpdateInfo, ARRAY_LEN(pbrDescriptorUpdateInfo));
            }
        }

        updateNodeDescriptorSets(model, std::vector<int32_t>(node.children.data(), node.children.data() + node.children.size()));
    }
}

static float s_Scale = 1.0f;

void updateNodeMatrix(LvnModel& model, const std::vector<int32_t>& nodes, std::vector<UniformData>& objectData, CameraView camera)
{
    for (const int32_t& nodeIndex : nodes)
    {
        const LvnNode& node = model.nodes[nodeIndex];
        if (node.mesh >= 0)
        {
            for (auto& primitive : model.meshes[node.mesh].primitives)
            {
                UniformData data{};
                LvnMat4 scaleMat = lvn::scale(LvnMat4(1.0f), LvnVec3(s_Scale));
                data.matrix = camera.matrix;
                data.model = scaleMat * getNodeMatrix(model, node);
                objectData.push_back(data);
            }
        }

        updateNodeMatrix(model, std::vector<int32_t>(node.children.data(), node.children.data() + node.children.size()), objectData, camera);
    }
}

void drawNode(LvnModel& model, const std::vector<int32_t>& nodes)
{
    for (const int32_t& nodeIndex : nodes)
    {
        const LvnNode& node = model.nodes[nodeIndex];
        if (node.mesh >= 0)
        {
            for (auto& primitive : model.meshes[node.mesh].primitives)
            {
                lvn::renderCmdBindDescriptorSets(window, pipeline, 0, 1, &primitive.descriptorSet);
                lvn::renderCmdBindVertexBuffer(window, 0, 1, &primitive.buffer, 0);
                lvn::renderCmdBindIndexBuffer(window, primitive.buffer, primitive.indexOffset);

                lvn::renderCmdDrawIndexed(window, primitive.indexCount);
            }
        }

        drawNode(model, std::vector<int32_t>(node.children.data(), node.children.data() + node.children.size()));
    }
}


int main()
{
    LvnContextCreateInfo lvnCreateInfo{};
    lvnCreateInfo.logging.enableLogging = true;
    lvnCreateInfo.logging.enableGraphicsApiDebugLogs = true;
    lvnCreateInfo.windowapi = Lvn_WindowApi_glfw;
    lvnCreateInfo.graphicsapi = Lvn_GraphicsApi_vulkan;
    lvnCreateInfo.rendering.frameBufferColorFormat = Lvn_TextureFormat_Unorm;
    lvnCreateInfo.rendering.maxFramesInFlight = 2;

    lvn::createContext(&lvnCreateInfo);


    // create window
    LvnWindowCreateInfo windowInfo{};
    windowInfo.title = "pbrScene";
    windowInfo.width = 800;
    windowInfo.height = 600;
    windowInfo.minWidth = 300;
    windowInfo.minHeight = 200;

    lvn::createWindow(&window, &windowInfo);

    lvn::windowSetVSync(window, true);

    // load model
    lvnmodel = lvn::loadModel("res/models/teapot/teapot.gltf");


    // create framebuffer
    LvnDepthImageFormat depthFormats[] =
    {
        Lvn_DepthImageFormat_Depth32Stencil8, Lvn_DepthImageFormat_Depth24Stencil8, Lvn_DepthImageFormat_Depth32, Lvn_DepthImageFormat_Depth16,
    };

    LvnDepthImageFormat supportedDepthFormat = lvn::findSupportedDepthImageFormat(depthFormats, ARRAY_LEN(depthFormats));

    // frame buffer attachments
    LvnFrameBufferColorAttachment frameBufferColorAttachment = { 0, Lvn_ColorImageFormat_RGBA32F };
    LvnFrameBufferDepthAttachment frameBufferDepthAttachment = { 1, supportedDepthFormat };

    LvnFrameBufferCreateInfo frameBufferCreateInfo{};
    frameBufferCreateInfo.width = 800;
    frameBufferCreateInfo.height = 600;
    frameBufferCreateInfo.sampleCount = Lvn_SampleCount_8_Bit;
    frameBufferCreateInfo.pColorAttachments = &frameBufferColorAttachment;
    frameBufferCreateInfo.colorAttachmentCount = 1;
    frameBufferCreateInfo.depthAttachment = &frameBufferDepthAttachment;
    frameBufferCreateInfo.textureMode = Lvn_TextureMode_ClampToEdge;
    frameBufferCreateInfo.textureFilter = Lvn_TextureFilter_Nearest;

    lvn::createFrameBuffer(&frameBuffer, &frameBufferCreateInfo);


    // create pipeline
    // pbr pipeline
    LvnShaderCreateInfo shaderCreateInfo{};
    shaderCreateInfo.vertexSrc = s_VertexShaderSrc;
    shaderCreateInfo.fragmentSrc = s_FragmentShaderSrc;

    lvn::createShaderFromSrc(&shader, &shaderCreateInfo);


    LvnVertexBindingDescription vertexBindingDescription{};
    vertexBindingDescription.stride = sizeof(LvnVertex);
    vertexBindingDescription.binding = 0;

    LvnVertexAttribute attributes[] =
    {
        { 0, 0, Lvn_AttributeFormat_Vec3_f32, 0 },                   // pos
        { 0, 1, Lvn_AttributeFormat_Vec4_f32, 3 * sizeof(float) },   // color
        { 0, 2, Lvn_AttributeFormat_Vec2_f32, 7 * sizeof(float) },   // texUV
        { 0, 3, Lvn_AttributeFormat_Vec3_f32, 9 * sizeof(float) },   // normal
        { 0, 4, Lvn_AttributeFormat_Vec3_f32, 12 * sizeof(float) },  // tangent
        { 0, 5, Lvn_AttributeFormat_Vec3_f32, 15 * sizeof(float) },  // bitangent
        { 0, 6, Lvn_AttributeFormat_Vec4_f32, 18 * sizeof(float) },  // joints
        { 0, 7, Lvn_AttributeFormat_Vec4_f32, 22 * sizeof(float) },  // weights
    };

    LvnVertexBindingDescription cubemapBindingDescription{};
    cubemapBindingDescription.stride = 3 * sizeof(float);
    cubemapBindingDescription.binding = 0;

    LvnVertexAttribute cubemapAttributes[] =
    {
        { 0, 0, Lvn_AttributeFormat_Vec3_f32, 0 },
    };

    LvnVertexBindingDescription fbVertexBindingDescription{};
    fbVertexBindingDescription.stride = 5 * sizeof(float);
    fbVertexBindingDescription.binding = 0;

    LvnVertexAttribute fbAttributes[] =
    {
        { 0, 0, Lvn_AttributeFormat_Vec3_f32, 0 },
        { 0, 1, Lvn_AttributeFormat_Vec2_f32, (3 * sizeof(float)) },
    };


    LvnDescriptorBinding uniformDescriptorBinding{};
    uniformDescriptorBinding.binding = 0;
    uniformDescriptorBinding.descriptorType = Lvn_DescriptorType_UniformBuffer;
    uniformDescriptorBinding.shaderStage = Lvn_ShaderStage_All;
    uniformDescriptorBinding.descriptorCount = 1;
    uniformDescriptorBinding.maxAllocations = 256;

    LvnDescriptorBinding uniformCubemapDescriptorBinding{};
    uniformCubemapDescriptorBinding.binding = 6;
    uniformCubemapDescriptorBinding.descriptorType = Lvn_DescriptorType_UniformBuffer;
    uniformCubemapDescriptorBinding.shaderStage = Lvn_ShaderStage_Vertex;
    uniformCubemapDescriptorBinding.descriptorCount = 1;
    uniformCubemapDescriptorBinding.maxAllocations = 1;

    LvnDescriptorBinding uniformModelDescriptorBinding{};
    uniformModelDescriptorBinding.binding = 1;
    uniformModelDescriptorBinding.descriptorType = Lvn_DescriptorType_UniformBuffer;
    uniformModelDescriptorBinding.shaderStage = Lvn_ShaderStage_Vertex;
    uniformModelDescriptorBinding.descriptorCount = 1;
    uniformModelDescriptorBinding.maxAllocations = 1;

    LvnDescriptorBinding uniformSkinMatrixDescriptorBinding{};
    uniformSkinMatrixDescriptorBinding.binding = 2;
    uniformSkinMatrixDescriptorBinding.descriptorType = Lvn_DescriptorType_StorageBuffer;
    uniformSkinMatrixDescriptorBinding.shaderStage = Lvn_ShaderStage_Vertex;
    uniformSkinMatrixDescriptorBinding.descriptorCount = 1;
    uniformSkinMatrixDescriptorBinding.maxAllocations = 1;

    LvnDescriptorBinding combinedImageDescriptorBinding{};
    combinedImageDescriptorBinding.binding = 1;
    combinedImageDescriptorBinding.descriptorType = Lvn_DescriptorType_ImageSampler;
    combinedImageDescriptorBinding.shaderStage = Lvn_ShaderStage_Fragment;
    combinedImageDescriptorBinding.descriptorCount = 1;
    combinedImageDescriptorBinding.maxAllocations = 256;

    std::vector<LvnDescriptorBinding> descriptorBindings =
    {
        uniformDescriptorBinding, uniformModelDescriptorBinding, uniformSkinMatrixDescriptorBinding,
        textureBinding(3, 256),
        textureBinding(4, 256),
        textureBinding(5, 256),
        textureBinding(6, 256),
    };

    LvnDescriptorLayoutCreateInfo descriptorLayoutCreateInfo{};
    descriptorLayoutCreateInfo.pDescriptorBindings = descriptorBindings.data();
    descriptorLayoutCreateInfo.descriptorBindingCount = descriptorBindings.size();
    descriptorLayoutCreateInfo.maxSets = 256;

    lvn::createDescriptorLayout(&descriptorLayout, &descriptorLayoutCreateInfo);

    LvnPipelineColorBlendAttachment colorAttachment{};
    colorAttachment.colorWriteMask.colorComponentR = true;
    colorAttachment.colorWriteMask.colorComponentG = true;
    colorAttachment.colorWriteMask.colorComponentB = true;
    colorAttachment.colorWriteMask.colorComponentA = true;
    colorAttachment.blendEnable = true;
    colorAttachment.srcColorBlendFactor = Lvn_ColorBlendFactor_SrcAlpha;
    colorAttachment.dstColorBlendFactor = Lvn_ColorBlendFactor_OneMinusSrcAlpha;
    colorAttachment.colorBlendOp = Lvn_ColorBlendOp_Add;
    colorAttachment.srcAlphaBlendFactor = Lvn_ColorBlendFactor_One;
    colorAttachment.dstAlphaBlendFactor = Lvn_ColorBlendFactor_Zero;
    colorAttachment.alphaBlendOp = Lvn_ColorBlendOp_Add;

    LvnPipelineSpecification pipelineSpec = lvn::configPipelineSpecificationInit();
    pipelineSpec.depthstencil.enableDepth = true;
    pipelineSpec.depthstencil.depthOpCompare = Lvn_CompareOp_LessOrEqual;
    pipelineSpec.rasterizer.cullMode = Lvn_CullFaceMode_Back;
    pipelineSpec.rasterizer.frontFace = Lvn_CullFrontFace_CCW;
    pipelineSpec.colorBlend.pColorBlendAttachments = &colorAttachment;
    pipelineSpec.colorBlend.colorBlendAttachmentCount = 1;

    LvnPipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.pipelineSpecification = &pipelineSpec;
    pipelineCreateInfo.pVertexBindingDescriptions = &vertexBindingDescription;
    pipelineCreateInfo.vertexBindingDescriptionCount = 1;
    pipelineCreateInfo.pVertexAttributes = attributes;
    pipelineCreateInfo.vertexAttributeCount = ARRAY_LEN(attributes);
    pipelineCreateInfo.pDescriptorLayouts = &descriptorLayout;
    pipelineCreateInfo.descriptorLayoutCount = 1;
    pipelineCreateInfo.shader = shader;
    pipelineCreateInfo.renderPass = lvn::frameBufferGetRenderPass(frameBuffer);
    pipelineCreateInfo.pipelineSpecification->multisampling.rasterizationSamples = Lvn_SampleCount_8_Bit;

    lvn::createPipeline(&pipeline, &pipelineCreateInfo);

    lvn::destroyShader(shader);


    // cubemap pipeline
    LvnShaderCreateInfo cubemapShaderCreateInfo{};
    cubemapShaderCreateInfo.vertexSrc = s_CubemapVertexShaderSrc;
    cubemapShaderCreateInfo.fragmentSrc = s_CubemapFragmentShaderSrc;

    lvn::createShaderFromSrc(&cubemapShader, &cubemapShaderCreateInfo);

    std::vector<LvnDescriptorBinding> cubemapDescriptorBinding =
    {
        uniformCubemapDescriptorBinding, combinedImageDescriptorBinding,
    };

    LvnDescriptorLayoutCreateInfo cubemapDescriptorLayoutCreateInfo{};
    cubemapDescriptorLayoutCreateInfo.pDescriptorBindings = cubemapDescriptorBinding.data();
    cubemapDescriptorLayoutCreateInfo.descriptorBindingCount = cubemapDescriptorBinding.size();
    cubemapDescriptorLayoutCreateInfo.maxSets = 1;

    lvn::createDescriptorLayout(&cubemapDescriptorLayout, &cubemapDescriptorLayoutCreateInfo);

    lvn::allocateDescriptorSet(&cubemapDescriptorSet, cubemapDescriptorLayout);

    pipelineCreateInfo.pDescriptorLayouts = &cubemapDescriptorLayout;
    pipelineCreateInfo.pVertexBindingDescriptions = &cubemapBindingDescription;
    pipelineCreateInfo.vertexBindingDescriptionCount = 1;
    pipelineCreateInfo.pVertexAttributes = cubemapAttributes;
    pipelineCreateInfo.vertexAttributeCount = ARRAY_LEN(cubemapAttributes);
    pipelineCreateInfo.shader = cubemapShader;
    pipelineCreateInfo.renderPass = lvn::frameBufferGetRenderPass(frameBuffer);
    pipelineCreateInfo.pipelineSpecification->depthstencil.depthOpCompare = Lvn_CompareOp_LessOrEqual;
    pipelineCreateInfo.pipelineSpecification->multisampling.rasterizationSamples = Lvn_SampleCount_8_Bit;
    pipelineCreateInfo.pipelineSpecification->colorBlend.colorBlendAttachmentCount = 0;
    pipelineCreateInfo.pipelineSpecification->rasterizer.cullMode = Lvn_CullFaceMode_Disable;

    lvn::createPipeline(&cubemapPipeline, &pipelineCreateInfo);

    lvn::destroyShader(cubemapShader);


    // framebuffer pipeline
    LvnShaderCreateInfo fbShaderCreateInfo{};
    fbShaderCreateInfo.vertexSrc = s_FbVertexShaderSrc;
    fbShaderCreateInfo.fragmentSrc = s_FbragmentShaderSrc;

    lvn::createShaderFromSrc(&fbShader, &fbShaderCreateInfo);

    LvnDescriptorLayoutCreateInfo fbDescriptorLayoutCreateInfo{};
    fbDescriptorLayoutCreateInfo.pDescriptorBindings = &combinedImageDescriptorBinding;
    fbDescriptorLayoutCreateInfo.descriptorBindingCount = 1;
    fbDescriptorLayoutCreateInfo.maxSets = 1;

    lvn::createDescriptorLayout(&fbDescriptorLayout, &fbDescriptorLayoutCreateInfo);

    LvnDescriptorSet* fbDescriptorSet;
    lvn::allocateDescriptorSet(&fbDescriptorSet, fbDescriptorLayout);

    pipelineCreateInfo.pDescriptorLayouts = &fbDescriptorLayout;
    pipelineCreateInfo.pVertexBindingDescriptions = &fbVertexBindingDescription;
    pipelineCreateInfo.vertexBindingDescriptionCount = 1;
    pipelineCreateInfo.pVertexAttributes = fbAttributes;
    pipelineCreateInfo.vertexAttributeCount = ARRAY_LEN(fbAttributes);
    pipelineCreateInfo.shader = fbShader;
    pipelineCreateInfo.renderPass = lvn::windowGetRenderPass(window);
    pipelineCreateInfo.pipelineSpecification->multisampling.rasterizationSamples = Lvn_SampleCount_1_Bit;
    pipelineCreateInfo.pipelineSpecification->colorBlend.colorBlendAttachmentCount = 0;

    lvn::createPipeline(&fbPipeline, &pipelineCreateInfo);

    lvn::destroyShader(fbShader);



    // framebuffer buffer
    LvnBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.type = Lvn_BufferType_Vertex;
    bufferCreateInfo.usage = Lvn_BufferUsage_Static;

    // NOTE: frame buffer vertex coords are different between vulkan and opengl, might fix in future
    LvnGraphicsApi graphicsapi = lvn::getGraphicsApi();

    if (graphicsapi == Lvn_GraphicsApi_vulkan)
    {
        bufferCreateInfo.data = s_FbVertices_vk;
        bufferCreateInfo.size = sizeof(s_FbVertices_vk);
    }
    if (graphicsapi == Lvn_GraphicsApi_opengl)
    {
        bufferCreateInfo.data = s_FbVertices_ogl;
        bufferCreateInfo.size = sizeof(s_FbVertices_ogl);
    }

    lvn::createBuffer(&fbBuffer, &bufferCreateInfo);

    // cubemap buffer
    std::vector<int8_t> cubemapBufferBin(sizeof(s_CubemapVertices) + sizeof(s_CubemapIndices));
    memcpy(cubemapBufferBin.data(), s_CubemapVertices, sizeof(s_CubemapVertices));
    memcpy(cubemapBufferBin.data() + sizeof(s_CubemapVertices), s_CubemapIndices, sizeof(s_CubemapIndices));

    LvnBufferCreateInfo cubemapBufferCreateInfo{};
    cubemapBufferCreateInfo.type = Lvn_BufferType_Vertex | Lvn_BufferType_Index;
    cubemapBufferCreateInfo.usage = Lvn_BufferUsage_Static;
    cubemapBufferCreateInfo.data = cubemapBufferBin.data();
    cubemapBufferCreateInfo.size = cubemapBufferBin.size();

    lvn::createBuffer(&cubemapBuffer, &cubemapBufferCreateInfo);


    // uniform buffer
    LvnBufferCreateInfo uniformBufferInfo{};
    uniformBufferInfo.type = Lvn_BufferType_Uniform;
    uniformBufferInfo.usage = Lvn_BufferUsage_Dynamic;
    uniformBufferInfo.size = sizeof(UniformData) * 256;

    lvn::createBuffer(&matrixUniformBuffer, &uniformBufferInfo);

    LvnBufferCreateInfo skinUniformBufferInfo{};
    skinUniformBufferInfo.type = Lvn_BufferType_Storage;
    skinUniformBufferInfo.usage = Lvn_BufferUsage_Dynamic;
    skinUniformBufferInfo.size = sizeof(LvnMat4);

    lvn::createBuffer(&defaultSkinUniformBuffer, &skinUniformBufferInfo);

    LvnBufferCreateInfo pbrUniformBufferInfo{};
    pbrUniformBufferInfo.type = Lvn_BufferType_Uniform;
    pbrUniformBufferInfo.usage = Lvn_BufferUsage_Dynamic;
    pbrUniformBufferInfo.size = sizeof(PbrUniformData);

    lvn::createBuffer(&pbrUniformBuffer, &pbrUniformBufferInfo);

    LvnBufferCreateInfo cubemapUniformBufferInfo{};
    cubemapUniformBufferInfo.type = Lvn_BufferType_Uniform;
    cubemapUniformBufferInfo.usage = Lvn_BufferUsage_Dynamic;
    cubemapUniformBufferInfo.size = sizeof(UniformData);

    lvn::createBuffer(&cubemapUniformBuffer, &cubemapUniformBufferInfo);


    // cubemap
    LvnCubemapCreateInfo cubemapCreateInfo{};
    cubemapCreateInfo.posx = lvn::loadImageData("res/cubemaps/sky/px.jpg", 4);
    cubemapCreateInfo.negx = lvn::loadImageData("res/cubemaps/sky/nx.jpg", 4);
    cubemapCreateInfo.posy = lvn::loadImageData("res/cubemaps/sky/py.jpg", 4);
    cubemapCreateInfo.negy = lvn::loadImageData("res/cubemaps/sky/ny.jpg", 4);
    cubemapCreateInfo.posz = lvn::loadImageData("res/cubemaps/sky/pz.jpg", 4);
    cubemapCreateInfo.negz = lvn::loadImageData("res/cubemaps/sky/nz.jpg", 4);

    lvn::createCubemap(&cubemap, &cubemapCreateInfo);


    // update descriptor sets
    std::vector<int32_t> roots = std::vector<int32_t>(lvnmodel.rootNodes.data(), lvnmodel.rootNodes.data() + lvnmodel.rootNodes.size());
    updateNodeDescriptorSets(lvnmodel, roots);

    LvnMat4 defualtSkin(1.0f);
    lvn::bufferUpdateData(defaultSkinUniformBuffer, &defualtSkin, sizeof(LvnMat4), 0);

    LvnTexture* frameBufferImage = lvn::frameBufferGetImage(frameBuffer, 0);
    LvnDescriptorUpdateInfo fbDescriptorTextureUpdateInfo{};
    fbDescriptorTextureUpdateInfo.descriptorType = Lvn_DescriptorType_ImageSampler;
    fbDescriptorTextureUpdateInfo.binding = 1;
    fbDescriptorTextureUpdateInfo.descriptorCount = 1;
    fbDescriptorTextureUpdateInfo.pTextureInfos = &frameBufferImage;

    lvn::updateDescriptorSetData(fbDescriptorSet, &fbDescriptorTextureUpdateInfo, 1);

    LvnTexture* cubemapTexture = lvn::cubemapGetTextureData(cubemap);

    LvnUniformBufferInfo cubemapBufferInfo{};
    cubemapBufferInfo.buffer = cubemapUniformBuffer;
    cubemapBufferInfo.range = sizeof(UniformData);
    cubemapBufferInfo.offset = 0;

    LvnDescriptorUpdateInfo cubemapDescriptorUniformUpdateInfo{};
    cubemapDescriptorUniformUpdateInfo.descriptorType = Lvn_DescriptorType_UniformBuffer;
    cubemapDescriptorUniformUpdateInfo.binding = 6;
    cubemapDescriptorUniformUpdateInfo.descriptorCount = 1;
    cubemapDescriptorUniformUpdateInfo.bufferInfo = &cubemapBufferInfo;

    LvnDescriptorUpdateInfo cubemapDescriptorTextureUpdateInfo{};
    cubemapDescriptorTextureUpdateInfo.descriptorType = Lvn_DescriptorType_ImageSampler;
    cubemapDescriptorTextureUpdateInfo.binding = 1;
    cubemapDescriptorTextureUpdateInfo.descriptorCount = 1;
    cubemapDescriptorTextureUpdateInfo.pTextureInfos = &cubemapTexture;

    std::vector<LvnDescriptorUpdateInfo> cubemapDescriptorUpdateInfo =
    {
        cubemapDescriptorUniformUpdateInfo, cubemapDescriptorTextureUpdateInfo,
    };

    lvn::updateDescriptorSetData(cubemapDescriptorSet, cubemapDescriptorUpdateInfo.data(), cubemapDescriptorUpdateInfo.size());


    CameraView camera{};
    camera.position = LvnVec3(0.0f, 0.0f, -1.0f);
    camera.orientation = LvnVec3(0.0f, 0.0f, 1.0f);
    camera.upVector = LvnVec3(0.0f, 1.0f, 0.0f);
    camera.data.fov = 60.0f;
    camera.data.zNear = 0.1f;
    camera.data.zFar = 100.0f;
    camera.data.aspectRatio = (float)lvn::windowGetSize(window).width / lvn::windowGetSize(window).height;


    EventData eventData{};
    eventData.fbDescriptorSet = fbDescriptorSet;
    eventData.frameBuffer = frameBuffer;

    lvn::windowSetEventCallback(window, eventsCallbackFn, &eventData);


    LvnTimer timer;
    int fps = 0;
    timer.begin();

    float oldTime = 0.0f;
    LvnTimer deltaTime;
    deltaTime.begin();

    UniformData uniformData{};
    PbrUniformData pbrData{};
    std::vector<UniformData> objectData;

    while (lvn::windowOpen(window))
    {
        lvn::windowUpdate(window);
        lvn::windowPollEvents();

        float timeNow = deltaTime.elapsed();
        float dt = timeNow - oldTime;
        oldTime = timeNow;
        orbitMovment(window, &camera, dt);
        scaleInput(window, &s_Scale, dt);


        lvn::renderBeginNextFrame(window);
        lvn::renderBeginCommandRecording(window);

        lvn::renderCmdBeginFrameBuffer(window, frameBuffer);
        lvn::frameBufferSetClearColor(frameBuffer, 0, 0.0f, 0.0f, 0.0f, 1.0f);

        objectData.clear();
        updateAnimation(lvnmodel, dt);
        updateNodeMatrix(lvnmodel, roots, objectData, camera);
        lvn::bufferUpdateData(matrixUniformBuffer, objectData.data(), objectData.size() * sizeof(UniformData), 0);

        pbrData.campPos = camera.position;
        pbrData.lightPos = lvn::vec3(cos(lvn::getContextTime()) * 3.0f, 0.0f, sin(lvn::getContextTime()) * 3.0f);
        pbrData.metalic = 0.5f;
        pbrData.roughness = 0.4f;
        pbrData.ambientOcclusion = 1.0f;
        pbrData.hasSkin = !lvnmodel.skins.empty();

        lvn::bufferUpdateData(pbrUniformBuffer, &pbrData, sizeof(PbrUniformData), 0);

        lvn::renderCmdBindPipeline(window, pipeline);

        drawNode(lvnmodel, roots);

        // draw cubemap
        lvn::mat4 projection = camera.projectionMatrix;
        lvn::mat4 view = lvn::mat4(lvn::mat3(camera.viewMatrix));
        uniformData.matrix = projection * view;

        lvn::bufferUpdateData(cubemapUniformBuffer, &uniformData, sizeof(UniformData), 0);
        lvn::renderCmdBindPipeline(window, cubemapPipeline);
        lvn::renderCmdBindDescriptorSets(window, cubemapPipeline, 0, 1, &cubemapDescriptorSet);

        lvn::renderCmdBindVertexBuffer(window, 0, 1, &cubemapBuffer, 0);
        lvn::renderCmdBindIndexBuffer(window, cubemapBuffer, sizeof(s_CubemapVertices));

        lvn::renderCmdDrawIndexed(window, ARRAY_LEN(s_CubemapIndices));

        lvn::renderCmdEndFrameBuffer(window, frameBuffer);


        // begin main render pass
        lvn::renderCmdBeginRenderPass(window, 0.0f, 0.0f, 0.0f, 1.0f);

        lvn::renderCmdBindPipeline(window, fbPipeline);
        lvn::renderCmdBindDescriptorSets(window, fbPipeline, 0, 1, &fbDescriptorSet);
        lvn::renderCmdBindVertexBuffer(window, 0, 1, &fbBuffer, 0);

        lvn::renderCmdDraw(window, 6);

        lvn::renderCmdEndRenderPass(window);
        lvn::renderEndCommandRecording(window);
        lvn::renderDrawSubmit(window);


        fps++;
        if (timer.elapsed() >= 1.0f)
        {
            LVN_TRACE("FPS: %d", fps);
            timer.reset();
            fps = 0;
        }
    }


    lvn::unloadModel(&lvnmodel);
    lvn::destroyCubemap(cubemap);
    lvn::destroyFrameBuffer(frameBuffer);

    lvn::destroyBuffer(fbBuffer);
    lvn::destroyBuffer(cubemapBuffer);

    lvn::destroyBuffer(cubemapUniformBuffer);
    lvn::destroyBuffer(matrixUniformBuffer);
    lvn::destroyBuffer(defaultSkinUniformBuffer);
    lvn::destroyBuffer(pbrUniformBuffer);


    lvn::destroyDescriptorLayout(cubemapDescriptorLayout);
    lvn::destroyDescriptorLayout(fbDescriptorLayout);
    lvn::destroyDescriptorLayout(descriptorLayout);

    lvn::destroyPipeline(cubemapPipeline);
    lvn::destroyPipeline(fbPipeline);
    lvn::destroyPipeline(pipeline);

    lvn::destroyWindow(window);
    lvn::terminateContext();

}
