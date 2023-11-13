#version 450

vec4 FULL_SCREEN_TRIANGLE[3] = vec4[]
(
    vec4(-1.0,  3.0, 0.0, 1.0),
    vec4( 3.0, -1.0, 0.0, 1.0),
    vec4(-1.0, -1.0, 0.0, 1.0)
);

void main()
{
    gl_Position = FULL_SCREEN_TRIANGLE[gl_VertexIndex];
}