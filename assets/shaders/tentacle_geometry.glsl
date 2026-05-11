#version 330

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

in vec3 vViewPos[];
uniform mat4 projection;
uniform float tubeRadius;
uniform float tubeRadiusTip;
uniform int stripLen;

out float gT;

void main() {
    vec3 A = vViewPos[0];
    vec3 B = vViewPos[1];

    float tA = float(gl_PrimitiveIDIn)     / float(stripLen - 1);
    float tB = float(gl_PrimitiveIDIn + 1) / float(stripLen - 1);

    float radiusA = tubeRadius * mix(1.0, tubeRadiusTip, tA);
    float radiusB = tubeRadius * mix(1.0, tubeRadiusTip, tB);

    vec3 dir = normalize(B - A);

    vec3 perp;
    if (abs(dot(dir, vec3(0.0, 0.0, -1.0))) > 0.999) {
        perp = normalize(cross(dir, vec3(0.0, 1.0, 0.0)));
    } else {
        perp = normalize(cross(dir, vec3(0.0, 0.0, -1.0)));
    }

    vec3 rightA = perp * radiusA;
    vec3 rightB = perp * radiusB;

    gT = tA; gl_Position = projection * vec4(A - rightA, 1.0); EmitVertex();
    gT = tA; gl_Position = projection * vec4(A + rightA, 1.0); EmitVertex();
    gT = tB; gl_Position = projection * vec4(B - rightB, 1.0); EmitVertex();
    gT = tB; gl_Position = projection * vec4(B + rightB, 1.0); EmitVertex();

    EndPrimitive();
}
