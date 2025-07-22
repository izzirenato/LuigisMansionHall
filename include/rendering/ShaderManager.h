#pragma once

#include "rendering/Shader.h"
#include "rendering/Model.h"
#include <map>
#include <memory>

enum class LightType
{
    DIRECTIONAL,
    POINT,
    SKYBOX
};

class ShaderManager
{
private:
    static std::map<MaterialType, std::unique_ptr<Shader>> materialShaders;
    static std::map<LightType, std::unique_ptr<Shader>> lightShaders;
    static std::unique_ptr<Shader> mirrorShader;
    static bool initialized;
    
public:
    static void initialize();
    static Shader* getShaderForMaterial(MaterialType type);
    static Shader* getShaderForLight(LightType type);
    static Shader* getMirrorShader();
    static void cleanup();
};