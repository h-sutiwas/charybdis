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

        Standard Phong combined with Fresnel and thickness
    */
    // Standard Phong
    /// ambient
    vec3 ambient = ambientStrength * lightColor;

    /// diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    /// specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;


    // Fresnel rim glow
    float fresnel = pow(1.0 - max(dot(norm, viewDir), 0.0), fresnelPower);

    // Thickness
    float thickness = max(dot(norm, viewDir), 0.0);
    float transmission = exp(-thickness * absorptionCoeff);

    // Backlight
    vec3 backlightDir = normalize(FragPos - lightPos);
    float backlight = max(dot(viewDir, -backlightDir), 0.0);
    vec3 backlightColor = lightColor * backlight * transmission;

    // Combination
    vec3 result = ((ambient + diffuse + specular) * objectColor + fresnel * lightColor * 0.5) + (backlightColor * objectColor);
    float alpha = 0.6 + 0.4 * (1.0 - transmission);
    FragColor = vec4(result, alpha);
}
