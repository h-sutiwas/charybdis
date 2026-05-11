#version 330 core

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

uniform float fresnelPower;
uniform float absorptionCoeff;
uniform float ambientStrength;

uniform float iridescence;
uniform float bioStrength;
uniform vec3  bioColor;
uniform float time;
uniform float contractionFreq;

void main() {
    /* Implement simplified translucency
        1. Fresnel rim glow
        2. Thickness-based transparency

        Standard Phong combined with Fresnel and thickness
    */
    vec3 norm     = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir  = normalize(viewPos  - FragPos);
    vec3 halfDir  = normalize(lightDir + viewDir);

    // Standard Phong
    /// ambient
    vec3 ambient = ambientStrength * lightColor;

    /// diffuse
    const float wrap = 0.5;
    float NdotL = dot(norm, lightDir);
    float wrapped = max((NdotL + wrap) / (1.0 + wrap), 0.0);
    vec3 diffuse = wrapped * lightColor;

    /// specular
    float specularStrength = 0.25;
    float spec = pow(max(dot(norm, halfDir), 0.0), 16.0);
    vec3 specular = specularStrength * spec * lightColor;


    // Fresnel rim glow
    float NdotV   = max(dot(norm, viewDir), 0.0);
    float facing  = smoothstep(0.0, 1.0, 1.0 - NdotV);
    float fresnel = pow(facing, fresnelPower);

    // Thickness
    float thickness = mix(1.0, NdotV, 0.5);
    float transmission = exp(-thickness * absorptionCoeff);

    // Backlight
    float backFacing = max(dot(viewDir, -lightDir), 0.0);
    float wrappedBack = pow(backFacing, 3.0);
    vec3 backlightColor = lightColor * wrappedBack * transmission;

    // Iridescent rim with hue cycle across the fresnel band
    vec3 iridescentColor = vec3(
        0.5 + 0.5 * cos(6.2832 * facing + 0.0),
        0.5 + 0.5 * cos(6.2832 * facing + 2.094),
        0.5 + 0.5 * cos(6.2832 * facing + 4.188)
    );
    vec3 rimColor = mix(lightColor, iridescentColor, iridescence);

    // Bioluminescent radial bands aka concentric rings expanding from apex
    float radial   = length(FragPos.xz);
    float bands    = sin(radial * 5.0 - time * contractionFreq * 6.2832) * 0.5 + 0.5;
    bands          = smoothstep(0.6, 1.0, bands);
    float bioMask  = smoothstep(0.0, 0.5, radial / 1.5);
    vec3 bioGlow   = bioColor * bands * bioMask * bioStrength * 0.4;

    // Combination
    vec3 lit = (ambient + diffuse + specular) * objectColor;
    vec3 rim = fresnel * rimColor * 0.35;
    vec3 sss = backlightColor * objectColor;
    vec3 result = lit + rim + sss + bioGlow;
    result = result / (result + vec3(1.0));
    result = pow(result, vec3(1.0 / 1.1));
    float alpha = mix(0.55, 0.95, NdotV);
    FragColor = vec4(result, alpha);
}