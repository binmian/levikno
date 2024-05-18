#version 460

layout (location = 0) in vec3 inPos;

layout (location = 0) out vec3 outTexCoords;

layout (binding = 0) uniform ObjectBuffer 
{
	mat4 matrix;
} ubo;

void main() 
{
	vec4 pos = ubo.matrix * vec4(inPos, 1.0);
    gl_Position = pos.xyww;
	outTexCoords = vec3(inPos.x, inPos.y, inPos.z);
}
