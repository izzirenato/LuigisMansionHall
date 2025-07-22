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

// Point lights (7 original + 7 mirrored = 14 total)
#define MAX_POINT_LIGHTS 14
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
    
    // Lambert diffuse with attenuation
    vec3 diffuse = (albedo / PI) * light.color * NdotL * attenuation;
    
    return diffuse;
}

void main() {
    vec3 albedo = useTextures ? texture(texture_diffuse1, TexCoords).rgb : vec3(1.0);
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPosition - FragPos);
    
    // Start with ambient and IBL
    vec3 iblDiffuse = texture(skybox, normal).rgb * iblStrength;
    vec3 ambient = (ambientLight + iblDiffuse) * albedo;
    
    // IBL specular reflection
    vec3 reflectDirIBL = reflect(-viewDir, normal);
    vec3 iblSpecular = texture(skybox, reflectDirIBL).rgb * iblStrength * 0.3;
    
    vec3 result = ambient + iblSpecular;
    
    // Add directional lights
    for (int i = 0; i < numDirLights && i < MAX_DIRECTIONAL_LIGHTS; i++) {
        result += calculateDirectionalLight(dirLights[i], normal, viewDir, albedo);
    }
    
    // Add point lights
    for (int i = 0; i < numPointLights && i < MAX_POINT_LIGHTS; i++) {
        result += calculatePointLight(pointLights[i], normal, FragPos, viewDir, albedo);
    }
    
    FragColor = vec4(result, 1.0);
}