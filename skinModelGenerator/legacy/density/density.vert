#version 410 core
// vertex shader for simple terrain demo

uniform int offset;

void main() {
    gl_Position = vec4(
    (gl_VertexID&1)*4 - 1,
    (gl_VertexID&2)*2 - 1,
                       0, 1);
}
