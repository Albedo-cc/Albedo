#version 450

layout(location = 0) out vec3 fragColor;

vec3 positions[6] = vec3[](
    vec3(0.0, -0.5, 0.5),
    vec3(0.5, 0.5,  0.5),
    vec3(-0.5, 0.5, 0.5),
    vec3(-0.2, -0.7, 0.8),
    vec3(0.3, 0.3,  0.8),
    vec3(-0.7, 0.3, 0.8)
);

vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

void main()
{
    gl_Position = vec4(positions[gl_VertexIndex], 1.0);
    int a = 5;
    fragColor = colors[gl_VertexIndex];
}