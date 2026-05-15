#version 330 core

in vec2 vUv;

out vec4 FragColor;

uniform vec3 bgTopColor;
uniform vec3 bgBottomColor;

void main() {
    vec3 color = mix(bgBottomColor, bgTopColor, vUv.y);
    FragColor = vec4(color, 1.0);
}