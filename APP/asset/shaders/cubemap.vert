#version 450

layout(location = 0) out vec3 cubemapUVW;

// layout(set = 0, binding = 0) uniform AppData
// {
//     mat4x4 mvp_model;
//     mat4x4 mvp_view;
//     mat4x4 mvp_projection;
// }appdata;


// [Vulkan Cubemap Order]
// Provide the images in the order
// +X, -X, +Y, -Y, +Z, -Z
// where the arrangement is a left-handed coordinate system with +Y up.
// Vertex positions for a unit cube

int cubeIndices[36] = int[](
    12, 13, 14, 14, 15, 12, // Right face (+X)
    8, 9, 10, 10, 11, 8,    // Left face  (-X)
    16, 17, 18, 18, 19, 16, // Top face   (+Y)
    20, 21, 22, 22, 23, 20, // Bottom face(-Y)
    0, 1, 2, 2, 3, 0,       // Front face (+Z)
    4, 5, 6, 6, 7, 4       // Back face  (-Z)
);

vec3 vertices[24] = vec3[](
    // Front face  (+Z)
    vec3(-1.0f, -1.0f,  1.0f),
    vec3( 1.0f, -1.0f,  1.0f),
    vec3( 1.0f,  1.0f,  1.0f),
    vec3(-1.0f,  1.0f,  1.0f),
    // Back face   (-Z)
    vec3(-1.0f, -1.0f, -1.0f),
    vec3( 1.0f, -1.0f, -1.0f),
    vec3( 1.0f,  1.0f, -1.0f),
    vec3(-1.0f,  1.0f, -1.0f),
    // Left face   (-X)
    vec3(-1.0f, -1.0f, -1.0f),
    vec3(-1.0f, -1.0f,  1.0f),
    vec3(-1.0f,  1.0f,  1.0f),
    vec3(-1.0f,  1.0f, -1.0f),
    // Right face  (+X)
    vec3( 1.0f, -1.0f, -1.0f),
    vec3( 1.0f, -1.0f,  1.0f),
    vec3( 1.0f,  1.0f,  1.0f),
    vec3( 1.0f,  1.0f, -1.0f),
    // Top face    (+Y)
    vec3(-1.0f,  1.0f, -1.0f),
    vec3(-1.0f,  1.0f,  1.0f),
    vec3( 1.0f,  1.0f,  1.0f),
    vec3( 1.0f,  1.0f, -1.0f),
    // Bottom face (-Y)
    vec3(-1.0f, -1.0f, -1.0f),
    vec3(-1.0f, -1.0f,  1.0f),
    vec3( 1.0f, -1.0f,  1.0f),
    vec3( 1.0f, -1.0f, -1.0f)
);

void main()
{
    int index = cubeIndices[gl_VertexIndex];
    cubemapUVW = vertices[index];

    gl_Position = //appdata.mvp_projection * 
                  //appdata.mvp_view       * 
                  //appdata.mvp_model      *
                  vec4(vertices[index], 1.0);
}