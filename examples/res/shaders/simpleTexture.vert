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
