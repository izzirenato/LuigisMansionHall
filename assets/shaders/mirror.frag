#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D dirtyTexture;
uniform vec3 viewPos;
uniform float time;
uniform float ghostEffect;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec4 dirtyMask = texture(dirtyTexture, TexCoords);
    
    float baseAlpha = 0.15;
    float dirtIntensity = 1.0 - dirtyMask.r;
    
    float ghostBreathing = sin(time * 1.5) * 0.02 + 0.98;
    float fresnel = pow(1.0 - max(dot(viewDir, norm), 0.0), 2.0);
    
    vec3 glassColor = vec3(0.9, 0.9, 1.0);
    vec3 dirtColor = vec3(0.5, 0.4, 0.3);
    
    vec3 finalColor = mix(glassColor, dirtColor, dirtIntensity * 0.4);
    finalColor += vec3(0.02, 0.02, 0.08) * fresnel * ghostBreathing;
    
    float finalAlpha = baseAlpha + dirtIntensity * 0.2 + fresnel * 0.1;
    
    FragColor = vec4(finalColor, finalAlpha * ghostBreathing);
}