#version 330

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

in vec3 vViewPos[];
uniform mat4 projection;
uniform float tubeRadius;

void main() {
    vec3 A = vViewPos[0];
    vec3 B = vViewPos[1];

    vec3 dir = normalize(B - A);

    vec3 right;
    if (abs(dot(dir, vec3(0.0, 0.0, -1.0))) > 0.999) {
        right = normalize(cross(dir, vec3(0.0, 1.0, 0.0))) * tubeRadius;
    } else {
        right = normalize(cross(dir, vec3(0.0, 0.0, -1.0))) * tubeRadius;
    }

    gl_Position = projection * vec4(A - right, 1.0);
    EmitVertex();
    gl_Position = projection * vec4(A + right, 1.0);
    EmitVertex();
    gl_Position = projection * vec4(B - right, 1.0);
    EmitVertex();
    gl_Position = projection * vec4(B + right, 1.0);
    EmitVertex();

    EndPrimitive();
}
