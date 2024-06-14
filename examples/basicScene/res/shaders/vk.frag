#version 460

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(binding = 1) uniform sampler2D texSampler;
layout(binding = 2) uniform sampler2D texSampler2;

void main()
{
	// outColor = texture(texSampler2, fragTexCoord);
	outColor = fragColor;
}
