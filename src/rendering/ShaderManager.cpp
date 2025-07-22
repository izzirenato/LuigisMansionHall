#include "rendering/ShaderManager.h"
#include <iostream>

// Inizializzazione statici
std::map<MaterialType, std::unique_ptr<Shader>> ShaderManager::materialShaders;
std::map<LightType, std::unique_ptr<Shader>> ShaderManager::lightShaders;
std::unique_ptr<Shader> ShaderManager::mirrorShader;
bool ShaderManager::initialized = false;

void ShaderManager::initialize()
{
    if (initialized) return;
    
    std::cout << "Initializing ShaderManager..." << std::endl;
    
    // Crea shader per Phong
    materialShaders[MaterialType::PHONG] = std::make_unique<Shader>(
        "assets/shaders/common.vert", 
        "assets/shaders/phong.frag"
    );
    std::cout << "Phong shader created" << std::endl;
    
    // Crea shader per Lambert
    materialShaders[MaterialType::LAMBERT] = std::make_unique<Shader>(
        "assets/shaders/common.vert", 
        "assets/shaders/lambert.frag"
    );
    std::cout << "Lambert shader created" << std::endl;
    
    // Crea shader per mirror
    mirrorShader = std::make_unique<Shader>(
        "assets/shaders/mirror.vert",
        "assets/shaders/mirror.frag"
    );
    std::cout << "Mirror shader created" << std::endl;
    
    // Crea shader per luci
    lightShaders[LightType::DIRECTIONAL] = std::make_unique<Shader>(
        "assets/shaders/common.vert",
        "assets/shaders/directional_light.frag"
    );
    std::cout << "Directional light shader created" << std::endl;
    
    lightShaders[LightType::POINT] = std::make_unique<Shader>(
        "assets/shaders/common.vert",
        "assets/shaders/point_light.frag"
    );
    std::cout << "Point light shader created" << std::endl;
    
    lightShaders[LightType::SKYBOX] = std::make_unique<Shader>(
        "assets/shaders/skybox.vert",
        "assets/shaders/skybox.frag"
    );
    std::cout << "Skybox shader created" << std::endl;
    
    initialized = true;
    std::cout << "ShaderManager initialized successfully" << std::endl;
}

Shader* ShaderManager::getShaderForMaterial(MaterialType type)
{
    if (!initialized) initialize();
    
    auto it = materialShaders.find(type);
    if (it != materialShaders.end()) {
        return it->second.get();
    }
    
    // Fallback to Phong
    std::cout << "Warning: Material type not found, falling back to Phong" << std::endl;
    return materialShaders[MaterialType::PHONG].get();
}

Shader* ShaderManager::getShaderForLight(LightType type)
{
    if (!initialized) initialize();
    
    auto it = lightShaders.find(type);
    if (it != lightShaders.end()) {
        return it->second.get();
    }
    
    // Fallback to directional light
    std::cout << "Warning: Light type not found, falling back to directional" << std::endl;
    return lightShaders[LightType::DIRECTIONAL].get();
}

Shader* ShaderManager::getMirrorShader()
{
    if (!initialized) initialize();
    return mirrorShader.get();
}

void ShaderManager::cleanup()
{
    materialShaders.clear();
    lightShaders.clear();
    mirrorShader.reset();
    initialized = false;
    std::cout << "ShaderManager cleaned up" << std::endl;
}