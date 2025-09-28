#version 460

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec2 fragTexCoord;

layout(binding = 1) uniform sampler2D inTexture;

void main()
{
    vec3 color = vec3(texture(inTexture, fragTexCoord));
    outColor = vec4(color, 1.0);
}
