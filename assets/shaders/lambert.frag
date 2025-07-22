#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform bool useTextures;
uniform samplerCube skybox;
uniform vec3 viewPosition;

// Skybox IBL
uniform float iblStrength;
uniform vec3 ambientLight;

// Directional lights (original + mirrored)
#define MAX_DIRECTIONAL_LIGHTS 2
struct DirectionalLight {
    vec3 direction;
    vec3 color;
    bool enabled;
};
uniform DirectionalLight dirLights[MAX_DIRECTIONAL_LIGHTS];
uniform int numDirLights;

// Point lights
#define MAX_POINT_LIGHTS 10
struct PointLight {
    vec3 position;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
    bool enabled;
};
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int numPointLights;

const float PI = 3.14159265359;

// Calculate directional light contribution
vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 albedo) {
    if (!light.enabled) return vec3(0.0);
    
    vec3 lightDir = normalize(-light.direction);
    float NdotL = max(dot(normal, lightDir), 0.0);
    
    // Lambert diffuse
    vec3 diffuse = (albedo / PI) * light.color * NdotL;
    
    return diffuse;
}

// Calculate point light contribution
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo) {
    if (!light.enabled) return vec3(0.0);
    
    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
    float NdotL = max(dot(normal, lightDir), 0.0);
    
    // Lambert diffuse
    vec3 diffuse = (albedo / PI) * light.color * NdotL * attenuation;
    
    return diffuse;
}

void main()
{
    vec3 textureColor = useTextures ? texture(texture_diffuse1, TexCoords).rgb : vec3(1.0);
    
    vec3 normalNorm = normalize(Normal);
    vec3 viewDir = normalize(viewPosition - FragPos);
    
    // IBL - Image Based Lighting from skybox
    vec3 iblDiffuse = texture(skybox, normalNorm).rgb * iblStrength;
    
    // Combine traditional ambient + IBL
    vec3 ambient = (ambientLight + iblDiffuse) * textureColor;
    
    vec3 result = ambient;
    
    // Add directional lights
    for(int i = 0; i < min(numDirLights, MAX_DIRECTIONAL_LIGHTS); i++) {
        result += calculateDirectionalLight(dirLights[i], normalNorm, viewDir, textureColor);
    }
    
    // Add point lights
    for(int i = 0; i < min(numPointLights, MAX_POINT_LIGHTS); i++) {
        result += calculatePointLight(pointLights[i], normalNorm, FragPos, viewDir, textureColor);
    }
    
    FragColor = vec4(result, 1.0);
}