#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform bool useTextures;

// Point light uniforms
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 viewPosition;
uniform vec3 ambientLight;

// Attenuation parameters
uniform float constant;
uniform float linear;
uniform float quadratic;

const float PI = 3.14159265359;

void main()
{
    vec3 textureColor = useTextures ? texture(texture_diffuse1, TexCoords).rgb : vec3(1.0);
    
    vec3 normalNorm = normalize(Normal);
    vec3 lightDir = normalize(lightPosition - FragPos);
    float NdotL = max(dot(normalNorm, lightDir), 0.0);
    
    // Calculate attenuation
    float distance = length(lightPosition - FragPos);
    float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));
    
    // Lambert formula: (albedo/π) * Li * (N·L) for point light
    vec3 diffuse = (textureColor / PI) * lightColor * NdotL * attenuation;
    
    // Ambient light (independent of material, but attenuated)
    vec3 ambient = ambientLight * textureColor * attenuation;
    
    vec3 finalColor = ambient + diffuse;
    FragColor = vec4(finalColor, 1.0);
}