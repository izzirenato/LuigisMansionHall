#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform float ambientStrength;
uniform float time;

void main()
{
    vec3 skyColor = texture(skybox, TexCoords).rgb;
    
    // Enhance skybox emission for IBL
    float emissionMultiplier = 2.0; // Increase base emission
    
    // Add subtle time-based variation (twinkling stars)
    float twinkle = sin(time * 3.0 + dot(TexCoords, vec3(123.456, 234.567, 345.678))) * 0.1 + 1.0;
    
    // Boost bright areas (stars) more than dark areas
    float luminance = dot(skyColor, vec3(0.299, 0.587, 0.114));
    float boost = mix(1.0, 3.0, luminance);
    
    // Final emission calculation
    vec3 finalColor = skyColor * ambientStrength * emissionMultiplier * twinkle * boost;
    
    FragColor = vec4(finalColor, 1.0);
}