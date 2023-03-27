#version 450

layout(binding = 0) uniform UniformBufferObject
{
    mat4x4 model;
    mat4x4 viewing;
} ubo;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main()
{
    gl_Position = ubo.viewing * ubo.model * vec4(inPosition, 0.0, 1.0);
    fragColor = inColor;
}