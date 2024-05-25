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

void main()
{
	ObjectData obj = ubo.objects[gl_BaseInstance];

	vec3 T = normalize(vec3(ubo.model * vec4(inTangent,   0.0)));
	vec3 B = normalize(vec3(ubo.model * vec4(inBitangent, 0.0)));
	vec3 N = normalize(vec3(ubo.model * vec4(inNormal,    0.0)));
	mat3 TBN = mat3(T, B, N);

	gl_Position = obj.u_Camera * vec4(inPos, 1.0);
	fragPos = inPos;
	fragColor = inColor;
	fragTexCoord = inTexCoord;
	fragNormal = inNormal;
}
