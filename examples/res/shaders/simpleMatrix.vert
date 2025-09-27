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
