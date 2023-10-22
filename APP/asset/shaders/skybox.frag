#version 450

// link to "cubemap.vert"
layout(location = 0) in vec3 cubemapUVW;
layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform samplerCube sampler_cube;

void main()
{
    outColor = texture(sampler_cube, cubemapUVW);
}