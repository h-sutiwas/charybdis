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

void main() {
    /* Implement simplified translucency
        1. Fresnel rim glow
        2. Thickness-based transparency

        Standard Phong mix with fresnel
    */
    // Standard Phong
    // ambient
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = diff * lightColor;

    // specular
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(-FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);

    // Fresnel rim glow
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 norm = normalize(Normal);
    float fresnel = pow(1.0f - max(dot(norm, viewDir), 0.0f), fresnelPower);

    // Thickness
    float thickness = max(dot(norm, viewDir), 0.0f);
    float transomission = exp(-thickness * absorptionCoeff);

    // Backlight
    vec3 backLightDir = normalize(FragPos - lightPos);
    float backlight = max(dot(viewDir, -backLightDir), 0.0f);
    vec3 backlightColor = lightColor * backlight * transmission;

    // Combination
    vec3 result = (ambient + diffuse + specular) * objectColor + fresnel * lightColor * 0.5 + backlightColor * objectColor;
    float alpha = 0.6 + 0.4 * (1.0 - transmission);
    FragColor = vec4(result, alpha);
}
