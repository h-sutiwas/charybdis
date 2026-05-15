#version 330

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;

out vec3 vViewPos;

void main() {
    vViewPos = vec3(view * model * vec4(aPos, 1.0));
}
