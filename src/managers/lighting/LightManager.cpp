#include "LightManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <string>

LightManager::LightManager() {
    // Default values
    ambientLight.color = Vector3f(0.06f, 0.06f, 0.07f);
    skyboxConfig.type = "procedural_night";
    skyboxConfig.iblStrength = 0.25f;
    skyboxConfig.ambientStrength = 0.5f;
}

LightManager::~LightManager() {
}

// Simple XML parsing helpers
std::string extractAttribute(const std::string& line, const std::string& attr) {
    size_t pos = line.find(attr + "=\"");
    if (pos == std::string::npos) return "";
    pos += attr.length() + 2;
    size_t end = line.find("\"", pos);
    if (end == std::string::npos) return "";
    return line.substr(pos, end - pos);
}

std::string extractTagContent(const std::string& line, const std::string& tag) {
    size_t start = line.find("<" + tag + ">");
    if (start == std::string::npos) return "";
    start += tag.length() + 2;
    size_t end = line.find("</" + tag + ">", start);
    if (end == std::string::npos) return "";
    return line.substr(start, end - start);
}

float parseFloat(const std::string& str, float defaultValue) {
    if (str.empty()) return defaultValue;
    try {
        return std::stof(str);
    } catch (...) {
        return defaultValue;
    }
}

bool parseBool(const std::string& str, bool defaultValue) {
    if (str.empty()) return defaultValue;
    return str == "true";
}

bool LightManager::loadFromXML(const std::string& xmlPath) {
    std::ifstream file(xmlPath);
    if (!file.is_open()) {
        std::cerr << "Error loading XML file: " << xmlPath << std::endl;
        return false;
    }
    
    std::string line;
    bool inLighting = false;
    
    directionalLights.clear();
    pointLights.clear();
    
    int pointLightIndex = 0;
    
    while (std::getline(file, line)) {
        // Remove leading/trailing whitespace
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        if (line.find("<Lighting>") != std::string::npos) {
            inLighting = true;
            continue;
        }
        
        if (line.find("</Lighting>") != std::string::npos) {
            inLighting = false;
            continue;
        }
        
        if (!inLighting) continue;
        
        // Parse AmbientLight
        if (line.find("<AmbientLight") != std::string::npos) {
            ambientLight.color.x = parseFloat(extractAttribute(line, "r"), 0.06f);
            ambientLight.color.y = parseFloat(extractAttribute(line, "g"), 0.06f);
            ambientLight.color.z = parseFloat(extractAttribute(line, "b"), 0.07f);
        }
        
        // Parse DirectionalLight
        if (line.find("<DirectionalLight") != std::string::npos) {
            DirectionalLight dirLight;
            dirLight.name = extractAttribute(line, "name");
            dirLight.enabled = parseBool(extractAttribute(line, "enabled"), true);
            dirLight.direction = Vector3f(0.0f, -1.0f, 0.0f);
            dirLight.color = Vector3f(1.0f, 1.0f, 1.0f);
            
            // Read next lines for Direction and Color
            std::string nextLine;
            while (std::getline(file, nextLine) && nextLine.find("</DirectionalLight>") == std::string::npos) {
                nextLine.erase(0, nextLine.find_first_not_of(" \t"));
                nextLine.erase(nextLine.find_last_not_of(" \t") + 1);
                
                if (nextLine.find("<Direction") != std::string::npos) {
                    dirLight.direction.x = parseFloat(extractAttribute(nextLine, "x"), 0.0f);
                    dirLight.direction.y = parseFloat(extractAttribute(nextLine, "y"), -1.0f);
                    dirLight.direction.z = parseFloat(extractAttribute(nextLine, "z"), 0.0f);
                }
                
                if (nextLine.find("<Color") != std::string::npos) {
                    dirLight.color.x = parseFloat(extractAttribute(nextLine, "r"), 1.0f);
                    dirLight.color.y = parseFloat(extractAttribute(nextLine, "g"), 1.0f);
                    dirLight.color.z = parseFloat(extractAttribute(nextLine, "b"), 1.0f);
                }
            }
            
            directionalLights.push_back(dirLight);
        }
        
        // Parse PointLight
        if (line.find("<PointLight") != std::string::npos) {
            PointLight pointLight;
            pointLight.name = extractAttribute(line, "name");
            pointLight.enabled = parseBool(extractAttribute(line, "enabled"), true);
            pointLight.dynamic = parseBool(extractAttribute(line, "dynamic"), false);
            pointLight.position = Vector3f(0.0f, 0.0f, 0.0f);
            pointLight.color = Vector3f(1.0f, 1.0f, 1.0f);
            pointLight.constant = 1.0f;
            pointLight.linear = 0.09f;
            pointLight.quadratic = 0.032f;
            pointLight.hasBreathingEffect = false;
            pointLight.breathingSpeed = 1.2f;
            pointLight.minIntensity = 0.4f;
            pointLight.maxIntensity = 1.0f;
            
            // Track special lights
            if (pointLight.name == "CameraLight") {
                cameraLightIndex = pointLightIndex;
            } else if (pointLight.name == "BooLight") {
                booLightIndex = pointLightIndex;
            }
            
            // Read next lines for Position, Color, attenuation, etc.
            std::string nextLine;
            while (std::getline(file, nextLine) && nextLine.find("</PointLight>") == std::string::npos) {
                nextLine.erase(0, nextLine.find_first_not_of(" \t"));
                nextLine.erase(nextLine.find_last_not_of(" \t") + 1);
                
                if (nextLine.find("<Position") != std::string::npos) {
                    pointLight.position.x = parseFloat(extractAttribute(nextLine, "x"), 0.0f);
                    pointLight.position.y = parseFloat(extractAttribute(nextLine, "y"), 0.0f);
                    pointLight.position.z = parseFloat(extractAttribute(nextLine, "z"), 0.0f);
                }
                
                if (nextLine.find("<Color") != std::string::npos) {
                    pointLight.color.x = parseFloat(extractAttribute(nextLine, "r"), 1.0f);
                    pointLight.color.y = parseFloat(extractAttribute(nextLine, "g"), 1.0f);
                    pointLight.color.z = parseFloat(extractAttribute(nextLine, "b"), 1.0f);
                }
                
                if (nextLine.find("<Constant>") != std::string::npos) {
                    pointLight.constant = parseFloat(extractTagContent(nextLine, "Constant"), 1.0f);
                }
                
                if (nextLine.find("<Linear>") != std::string::npos) {
                    pointLight.linear = parseFloat(extractTagContent(nextLine, "Linear"), 0.09f);
                }
                
                if (nextLine.find("<Quadratic>") != std::string::npos) {
                    pointLight.quadratic = parseFloat(extractTagContent(nextLine, "Quadratic"), 0.032f);
                }
                
                if (nextLine.find("<BreathingEffect") != std::string::npos) {
                    pointLight.hasBreathingEffect = parseBool(extractAttribute(nextLine, "enabled"), false);
                    pointLight.breathingSpeed = parseFloat(extractAttribute(nextLine, "speed"), 1.2f);
                    pointLight.minIntensity = parseFloat(extractAttribute(nextLine, "minIntensity"), 0.4f);
                    pointLight.maxIntensity = parseFloat(extractAttribute(nextLine, "maxIntensity"), 1.0f);
                }
            }
            
            pointLights.push_back(pointLight);
            pointLightIndex++;
        }
        
        // Parse IBLStrength
        if (line.find("<IBLStrength>") != std::string::npos) {
            skyboxConfig.iblStrength = parseFloat(extractTagContent(line, "IBLStrength"), 0.25f);
        }
        
        // Parse AmbientStrength
        if (line.find("<AmbientStrength>") != std::string::npos) {
            skyboxConfig.ambientStrength = parseFloat(extractTagContent(line, "AmbientStrength"), 0.5f);
        }
    }
    
    file.close();
    
    std::cout << "Loaded lighting configuration:" << std::endl;
    std::cout << "- Ambient light: " << ambientLight.color.x << ", " << ambientLight.color.y << ", " << ambientLight.color.z << std::endl;
    std::cout << "- Directional lights: " << directionalLights.size() << std::endl;
    std::cout << "- Point lights: " << pointLights.size() << std::endl;
    std::cout << "- Camera light index: " << cameraLightIndex << std::endl;
    std::cout << "- Boo light index: " << booLightIndex << std::endl;
    
    return true;
}

float LightManager::calculateBreathingIntensity(float time, float speed, float minIntensity, float maxIntensity) {
    float breathingValue = sin(time * speed) * 0.5f + 0.5f; // 0 to 1
    return minIntensity + (maxIntensity - minIntensity) * breathingValue;
}

void LightManager::applyToShader(Shader* shader, const Vector3f& cameraPosition, float breathingTime) {
    // Set ambient light
    shader->setVec3("ambientLight", ambientLight.color.x, ambientLight.color.y, ambientLight.color.z);
    
    // Set directional lights
    shader->setInt("numDirLights", directionalLights.size());
    for (size_t i = 0; i < directionalLights.size(); i++) {
        std::string baseName = "dirLights[" + std::to_string(i) + "]";
        shader->setVec3(baseName + ".direction", 
            directionalLights[i].direction.x, 
            directionalLights[i].direction.y, 
            directionalLights[i].direction.z);
        shader->setVec3(baseName + ".color", 
            directionalLights[i].color.x, 
            directionalLights[i].color.y, 
            directionalLights[i].color.z);
        shader->setBool(baseName + ".enabled", directionalLights[i].enabled);
    }
    
    // Set point lights
    int activePointLights = 0;
    for (size_t i = 0; i < pointLights.size(); i++) {
        if (!pointLights[i].enabled) continue;
        
        std::string baseName = "pointLights[" + std::to_string(activePointLights) + "]";
        
        // Update position for dynamic lights 
        Vector3f position = pointLights[i].position;
        if (pointLights[i].dynamic && i == cameraLightIndex) {
            position = cameraPosition;  // Posiziona la luce della camera sulla camera
        }
        
        shader->setVec3(baseName + ".position", position.x, position.y, position.z);
        
        // Apply breathing effect if enabled
        Vector3f color = pointLights[i].color;
        if (pointLights[i].hasBreathingEffect) {
            float intensity = calculateBreathingIntensity(breathingTime, 
                pointLights[i].breathingSpeed, 
                pointLights[i].minIntensity, 
                pointLights[i].maxIntensity);
            color = color * intensity;
        }
        
        shader->setVec3(baseName + ".color", color.x, color.y, color.z);
        shader->setFloat(baseName + ".constant", pointLights[i].constant);
        shader->setFloat(baseName + ".linear", pointLights[i].linear);
        shader->setFloat(baseName + ".quadratic", pointLights[i].quadratic);
        shader->setBool(baseName + ".enabled", true);
        
        activePointLights++;
    }
    
    shader->setInt("numPointLights", activePointLights);
    
    // Set skybox/IBL strength
    shader->setFloat("iblStrength", skyboxConfig.iblStrength);
}

void LightManager::applyToShaderWithBoo(Shader* shader, const Vector3f& cameraPosition, const Vector3f& booPosition, float breathingTime) {
    // Set ambient light
    shader->setVec3("ambientLight", ambientLight.color.x, ambientLight.color.y, ambientLight.color.z);
    
    // Set directional lights
    shader->setInt("numDirLights", directionalLights.size());
    for (size_t i = 0; i < directionalLights.size(); i++) {
        std::string baseName = "dirLights[" + std::to_string(i) + "]";
        shader->setVec3(baseName + ".direction", 
            directionalLights[i].direction.x, 
            directionalLights[i].direction.y, 
            directionalLights[i].direction.z);
        shader->setVec3(baseName + ".color", 
            directionalLights[i].color.x, 
            directionalLights[i].color.y, 
            directionalLights[i].color.z);
        shader->setBool(baseName + ".enabled", directionalLights[i].enabled);
    }
    
    // Set point lights - INCLUDI SEMPRE ENTRAMBE LE LUCI DINAMICHE
    int activePointLights = 0;
    for (size_t i = 0; i < pointLights.size(); i++) {
        // Forza l'abilitazione delle luci dinamiche
        bool lightEnabled = pointLights[i].enabled;
        if (i == cameraLightIndex || i == booLightIndex) {
            lightEnabled = true;
        }
        
        if (!lightEnabled) continue;
        
        std::string baseName = "pointLights[" + std::to_string(activePointLights) + "]";
        
        // Update position for dynamic lights
        Vector3f position = pointLights[i].position;
        if (pointLights[i].dynamic) {
            if (i == cameraLightIndex) {
                position = cameraPosition;
            } else if (i == booLightIndex) {
                position = booPosition;
            }
        }
        
        shader->setVec3(baseName + ".position", position.x, position.y, position.z);
        
        // Apply breathing effect if enabled
        Vector3f color = pointLights[i].color;
        if (pointLights[i].hasBreathingEffect) {
            float intensity = calculateBreathingIntensity(breathingTime, 
                pointLights[i].breathingSpeed, 
                pointLights[i].minIntensity, 
                pointLights[i].maxIntensity);
            color = color * intensity;
        }
        
        shader->setVec3(baseName + ".color", color.x, color.y, color.z);
        shader->setFloat(baseName + ".constant", pointLights[i].constant);
        shader->setFloat(baseName + ".linear", pointLights[i].linear);
        shader->setFloat(baseName + ".quadratic", pointLights[i].quadratic);
        shader->setBool(baseName + ".enabled", true);
        
        activePointLights++;
    }
    
    shader->setInt("numPointLights", activePointLights);
    
    // Set skybox/IBL strength
    shader->setFloat("iblStrength", skyboxConfig.iblStrength);
}

void LightManager::applyToShaderWithCamera(Shader* shader, const Vector3f& cameraPosition, const Vector3f& cameraDirection, float breathingTime) {
    // Set ambient light
    shader->setVec3("ambientLight", ambientLight.color.x, ambientLight.color.y, ambientLight.color.z);
    
    // Set directional lights
    shader->setInt("numDirLights", directionalLights.size());
    for (size_t i = 0; i < directionalLights.size(); i++) {
        std::string baseName = "dirLights[" + std::to_string(i) + "]";
        shader->setVec3(baseName + ".direction", 
            directionalLights[i].direction.x, 
            directionalLights[i].direction.y, 
            directionalLights[i].direction.z);
        shader->setVec3(baseName + ".color", 
            directionalLights[i].color.x, 
            directionalLights[i].color.y, 
            directionalLights[i].color.z);
        shader->setBool(baseName + ".enabled", directionalLights[i].enabled);
    }
    
    // Set point lights
    int activePointLights = 0;
    for (size_t i = 0; i < pointLights.size(); i++) {
        if (!pointLights[i].enabled) continue;
        
        std::string baseName = "pointLights[" + std::to_string(activePointLights) + "]";
        
        // Update position for dynamic lights 
        Vector3f position = pointLights[i].position;
        if (pointLights[i].dynamic && i == cameraLightIndex) {
            position = cameraPosition;
        }
        
        shader->setVec3(baseName + ".position", position.x, position.y, position.z);
        
        // Apply breathing effect if enabled
        Vector3f color = pointLights[i].color;
        if (pointLights[i].hasBreathingEffect) {
            float intensity = calculateBreathingIntensity(breathingTime, 
                pointLights[i].breathingSpeed, 
                pointLights[i].minIntensity, 
                pointLights[i].maxIntensity);
            color = color * intensity;
        }
        
        shader->setVec3(baseName + ".color", color.x, color.y, color.z);
        shader->setFloat(baseName + ".constant", pointLights[i].constant);
        shader->setFloat(baseName + ".linear", pointLights[i].linear);
        shader->setFloat(baseName + ".quadratic", pointLights[i].quadratic);
        shader->setBool(baseName + ".enabled", true);
        
        activePointLights++;
    }
    shader->setInt("numPointLights", activePointLights);
    
    // Set spot lights (inclusa la camera light come spotlight)
    int activeSpotLights = 0;
    for (size_t i = 0; i < spotLights.size(); i++) {
        if (!spotLights[i].enabled) continue;
        
        std::string baseName = "spotLights[" + std::to_string(activeSpotLights) + "]";
        
        // Update position and direction for dynamic lights
        Vector3f position = spotLights[i].position;
        Vector3f direction = spotLights[i].direction;
        if (spotLights[i].dynamic && i == cameraSpotLightIndex) {
            position = cameraPosition;
            direction = cameraDirection;
        }
        
        shader->setVec3(baseName + ".position", position.x, position.y, position.z);
        shader->setVec3(baseName + ".direction", direction.x, direction.y, direction.z);
        
        // Apply breathing effect if enabled
        Vector3f color = spotLights[i].color;
        if (spotLights[i].hasBreathingEffect) {
            float intensity = calculateBreathingIntensity(breathingTime, 
                spotLights[i].breathingSpeed, 
                spotLights[i].minIntensity, 
                spotLights[i].maxIntensity);
            color = color * intensity;
        }
        
        shader->setVec3(baseName + ".color", color.x, color.y, color.z);
        shader->setFloat(baseName + ".constant", spotLights[i].constant);
        shader->setFloat(baseName + ".linear", spotLights[i].linear);
        shader->setFloat(baseName + ".quadratic", spotLights[i].quadratic);
        shader->setFloat(baseName + ".cutOff", spotLights[i].cutOff);
        shader->setFloat(baseName + ".outerCutOff", spotLights[i].outerCutOff);
        shader->setBool(baseName + ".enabled", true);
        
        activeSpotLights++;
    }
    shader->setInt("numSpotLights", activeSpotLights);
    
    // Set skybox/IBL strength
    shader->setFloat("iblStrength", skyboxConfig.iblStrength);
}

void LightManager::enableBooLight(bool enable) {
    if (booLightIndex >= 0 && booLightIndex < pointLights.size()) {
        pointLights[booLightIndex].enabled = enable;
    }
}