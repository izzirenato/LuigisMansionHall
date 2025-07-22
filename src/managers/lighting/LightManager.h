#pragma once
#include "math/Vector3f.h"
#include "rendering/Shader.h"
#include <vector>
#include <string>

struct DirectionalLight {
    std::string name;
    Vector3f direction;
    Vector3f color;
    bool enabled;
};

struct PointLight {
    std::string name;
    Vector3f position;
    Vector3f color;
    float constant;
    float linear;
    float quadratic;
    bool enabled;
    bool dynamic;
    bool hasBreathingEffect;
    float breathingSpeed;
    float minIntensity;
    float maxIntensity;
};

struct SpotLight {
    std::string name;
    Vector3f position;
    Vector3f direction;
    Vector3f color;
    float constant;
    float linear;
    float quadratic;
    float cutOff;           // Angolo interno del cono
    float outerCutOff;      // Angolo esterno del cono
    bool enabled;
    bool dynamic;
    bool hasBreathingEffect;
    float breathingSpeed;
    float minIntensity;
    float maxIntensity;
};

struct AmbientLight {
    Vector3f color;
};

struct SkyboxConfig {
    std::string type;
    float iblStrength;
    float ambientStrength;
};

class LightManager {
private:
    std::vector<DirectionalLight> directionalLights;
    std::vector<PointLight> pointLights;
    std::vector<SpotLight> spotLights;
    AmbientLight ambientLight;
    SkyboxConfig skyboxConfig;
    
    int cameraLightIndex = -1;
    int booLightIndex = -1;
    int cameraSpotLightIndex = -1;  // Nuovo indice per spotlight della camera
    
    float calculateBreathingIntensity(float time, float speed, float minIntensity, float maxIntensity);
    
public:
    LightManager();
    ~LightManager();
    
    bool loadFromXML(const std::string& xmlPath);
    void applyToShader(Shader* shader, const Vector3f& cameraPosition, float breathingTime);
    void applyToShaderWithBoo(Shader* shader, const Vector3f& cameraPosition, const Vector3f& booPosition, float breathingTime);
    void applyToShaderWithCamera(Shader* shader, const Vector3f& cameraPosition, const Vector3f& cameraDirection, float breathingTime);
    void enableBooLight(bool enable);
};