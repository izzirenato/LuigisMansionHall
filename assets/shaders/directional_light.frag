#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform bool useTextures;

// Directional light uniforms
uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform vec3 viewPosition;
uniform vec3 ambientLight;

const float PI = 3.14159265359;

void main()
{
    vec3 textureColor = useTextures ? texture(texture_diffuse1, TexCoords).rgb : vec3(1.0);
    
    vec3 normalNorm = normalize(Normal);
    vec3 lightDir = normalize(-lightDirection); // Negative because we want direction TO the light
    float NdotL = max(dot(normalNorm, lightDir), 0.0);
    
    // Lambert formula: (albedo/π) * Li * (N·L) for directional light
    vec3 diffuse = (textureColor / PI) * lightColor * NdotL;
    
    // Ambient light (independent of material)
    vec3 ambient = ambientLight * textureColor;
    
    vec3 finalColor = ambient + diffuse;
    FragColor = vec4(finalColor, 1.0);
}