#version 330

in float gT;

out vec4 FragColor;

uniform vec3 tentacleColor;

void main() {
    float brightness = mix(1.0, 0.5, gT);
    float alpha      = mix(0.85, 0.25, gT);
    FragColor = vec4(tentacleColor * brightness, alpha);
}
