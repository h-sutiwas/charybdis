#version 330 core

in float vT;

out vec4 FragColor;

uniform vec3 strandColor;

void main() {
    float brightness = mix(1.0, 0.4, vT);
    float alpha      = mix(0.70, 0.15, vT);
    FragColor = vec4(strandColor * brightness, alpha);
}