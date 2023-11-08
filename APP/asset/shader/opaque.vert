#version 450

layout(location = 0) out vec3 fragColor;

layout(set = 0, binding = 0) uniform GlobalUBO_Camera
{
    mat4x4 view_matrix;
    mat4x4 proj_matrix;
}ubo_camera;

vec3 positions[6] = vec3[](
    vec3(0.0, 0.5, 0.5),
    vec3(0.5, -0.5,  0.5),
    vec3(-0.5, -0.5, 0.5),

    vec3(-0.2, 0.7, 0.8),
    vec3(0.3, -0.3,  0.8),
    vec3(-0.7, -0.3, 0.8)
);

vec3 colors[6] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0),

    vec3(1.0, 0.0, 0.0),
    vec3(1.0, 0.0, 0.0),
    vec3(1.0, 0.0, 0.0)
);

void main()
{
    mat4x4 a = mat4x4(1.0);

    gl_Position = ubo_camera.proj_matrix *
                  ubo_camera.view_matrix *
                  vec4(positions[gl_VertexIndex], 1.0);

    fragColor = colors[gl_VertexIndex]; 
}