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

// Calculate directional light contribution with Phong shading
vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 albedo) {
    if (!light.enabled) return vec3(0.0);
    
    vec3 lightDir = normalize(-light.direction);
    float NdotL = max(dot(normal, lightDir), 0.0);
    
    // Diffuse (senza divisione per PI - è Phong, non PBR)
    vec3 diffuse = albedo * light.color * NdotL;
    
    // Phong specular
    vec3 specular = vec3(0.0);
    if (NdotL > 0.0) {
        vec3 reflectDir = reflect(-lightDir, normal);
        float RdotV = max(dot(viewDir, reflectDir), 0.0);
        float specStrength = 0.5;
        float shininess = 32.0;
        float spec = pow(RdotV, shininess);
        specular = spec * specStrength * light.color;
    }
    
    return diffuse + specular;
}

// Calculate point light contribution with Phong shading
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo) {
    if (!light.enabled) return vec3(0.0);
    
    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);
    
    // Calcolo attenuazione con controllo per divisione per zero
    float denominator = light.constant + light.linear * distance + light.quadratic * (distance * distance);
    float attenuation = 1.0 / max(denominator, 0.001); // Evita divisione per zero
    
    float NdotL = max(dot(normal, lightDir), 0.0);
    
    // Diffuse con attenuazione (senza divisione per PI)
    vec3 diffuse = albedo * light.color * NdotL * attenuation;
    
    // Phong specular con attenuazione
    vec3 specular = vec3(0.0);
    if (NdotL > 0.0) {
        vec3 reflectDir = reflect(-lightDir, normal);
        float RdotV = max(dot(viewDir, reflectDir), 0.0);
        float specStrength = 0.5;
        float shininess = 32.0;
        float spec = pow(RdotV, shininess);
        specular = spec * specStrength * light.color * attenuation;
    }
    
    return diffuse + specular;
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
        vec3 dirContrib = calculateDirectionalLight(dirLights[i], normal, viewDir, albedo);
        result += dirContrib;
    }
    
    // Add point lights
    for (int i = 0; i < numPointLights && i < MAX_POINT_LIGHTS; i++) {
        vec3 pointContrib = calculatePointLight(pointLights[i], normal, FragPos, viewDir, albedo);
        result += pointContrib;
    }
    
    FragColor = vec4(result, 1.0);
}