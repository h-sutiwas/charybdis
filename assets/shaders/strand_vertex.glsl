#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform int stripStart;
uniform int stripLen;

out float vT;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    int local = gl_VertexID - stripStart;
    vT = float(local) / float(stripLen - 1);
}