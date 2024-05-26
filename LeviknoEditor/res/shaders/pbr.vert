#version 460

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;
layout(location = 4) in vec3 inTangent;
layout(location = 5) in vec3 inBitangent;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec4 fragColor;
layout(location = 2) out vec2 fragTexCoord;
layout(location = 3) out vec3 fragNormal;

struct ObjectData
{
	mat4 u_Camera;
	mat4 model;
};

layout(std140, binding = 0) readonly buffer UniformBufferObject
{
	ObjectData objects[];
} ubo;

layout(binding = 1) uniform ObjectBuffer
{
	vec3 camPos;
	vec3 lightPos;
	float metalic;
	float roughness;
	float ambientOcclusion;
} pbrUbo;

void main()
{
	ObjectData obj = ubo.objects[gl_BaseInstance];

	fragPos = vec3(obj.model * vec4(inPos, 1.0));
	fragColor = inColor;
	fragTexCoord = inTexCoord;
	fragNormal = mat3(transpose(inverse(obj.model))) * inNormal;

	gl_Position = obj.u_Camera * vec4(fragPos, 1.0);
}
