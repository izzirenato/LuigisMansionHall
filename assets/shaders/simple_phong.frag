#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform bool useTextures;

uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 viewPosition;
uniform vec3 ambientLight;

void main()
{
    vec3 textureColor = useTextures ? texture(texture_diffuse1, TexCoords).rgb : vec3(1.0);
    
    vec3 normalNorm = normalize(Normal);
    vec3 lightDir = normalize(lightPosition - FragPos);
    float NdotL = max(dot(normalNorm, lightDir), 0.0);
    
    // Simple diffuse
    vec3 diffuse = NdotL * lightColor * textureColor;
    
    // PHONG SPECULAR
    vec3 viewDir = normalize(viewPosition - FragPos);
    vec3 reflectDir = reflect(-lightDir, normalNorm);
    float RdotV = max(dot(viewDir, reflectDir), 0.0);
    
    vec3 specular = vec3(0.0);
    if(NdotL > 0.0) {
        float specStrength = 0.5;
        float shininess = 32.0;
        float spec = pow(RdotV, shininess);
        specular = spec * specStrength * lightColor;
    }
    
    // Simple ambient without IBL
    vec3 ambient = ambientLight * textureColor;
    
    vec3 finalColor = ambient + diffuse + specular;
    FragColor = vec4(finalColor, 1.0);
}