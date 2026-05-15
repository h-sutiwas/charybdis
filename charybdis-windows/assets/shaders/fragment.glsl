#version 330 core
out vec4 FragColor;

uniform vec4 color1;
uniform vec4 color2;

uniform float time;

void main() {
    float blend = sin(time * 5.0f) * 0.5f + 0.5f;

    FragColor = mix(color1, color2, blend);
}