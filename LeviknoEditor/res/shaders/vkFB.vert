#version 450

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;
layout(location = 4) in vec3 inTangent;
layout(location = 5) in vec3 inBitangent;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;


layout(binding = 0) uniform UniformBufferObject
{
    mat4 u_Camera;
} ubo;

void main()
{
	gl_Position = ubo.u_Camera * vec4(inPos, 1.0);
	fragColor = inColor;
	fragTexCoord = inTexCoord;
}
