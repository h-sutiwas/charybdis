#version 330

out vec4 FragColor;

uniform vec3 tentacleColor;

void main() {
    FragColor = vec4(tentacleColor, 0.85);
}
