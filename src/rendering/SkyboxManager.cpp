#include "SkyboxManager.h"
#include "../../include/rendering/ShaderManager.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <algorithm>

SkyboxManager::SkyboxManager() : skyboxVAO(0), skyboxVBO(0), skyboxTexture(0), initialized(false) {
}

SkyboxManager::~SkyboxManager() {
    cleanup();
}

bool SkyboxManager::initialize() {
    if (initialized) return true;
    
    // Setup skybox geometry
    float scale = 100.0f;
    float skyboxVertices[] = {
        // positions          
        -scale,  scale, -scale,
        -scale, -scale, -scale,
         scale, -scale, -scale,
         scale, -scale, -scale,
         scale,  scale, -scale,
        -scale,  scale, -scale,

        -scale, -scale,  scale,
        -scale, -scale, -scale,
        -scale,  scale, -scale,
        -scale,  scale, -scale,
        -scale,  scale,  scale,
        -scale, -scale,  scale,

         scale, -scale, -scale,
         scale, -scale,  scale,
         scale,  scale,  scale,
         scale,  scale,  scale,
         scale,  scale, -scale,
         scale, -scale, -scale,

        -scale, -scale,  scale,
        -scale,  scale,  scale,
         scale,  scale,  scale,
         scale,  scale,  scale,
         scale, -scale,  scale,
        -scale, -scale,  scale,

        -scale,  scale, -scale,
         scale,  scale, -scale,
         scale,  scale,  scale,
         scale,  scale,  scale,
        -scale,  scale,  scale,
        -scale,  scale, -scale,

        -scale, -scale, -scale,
        -scale, -scale,  scale,
         scale, -scale, -scale,
         scale, -scale, -scale,
        -scale, -scale,  scale,
         scale, -scale,  scale
    };

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    
    // Create skybox texture
    skyboxTexture = createNightSkyCubemap();
    
    initialized = true;
    return true;
}

void SkyboxManager::render(Camera* camera) {
    if (!initialized || !camera) return;
    
    // Render skybox as a fixed dome around the house
    glDepthFunc(GL_LEQUAL);
    
    Shader* skyboxShader = ShaderManager::getShaderForLight(LightType::SKYBOX);
    if (!skyboxShader) return;
    
    skyboxShader->use();
    
    // Use normal view matrix (skybox will be fixed in world space)
    Matrix4f view = camera->viewMatrix;
    skyboxShader->setMat4("view", &view.a11);
    
    Matrix4f proj = camera->projectionMatrix;
    proj.transpose();
    skyboxShader->setMat4("projection", &proj.a11);
    
    skyboxShader->setFloat("ambientStrength", 0.5f);
    skyboxShader->setFloat("time", (float)glfwGetTime());
    
    // Bind skybox texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
    skyboxShader->setInt("skybox", 0);
    
    // Render skybox
    glBindVertexArray(skyboxVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    
    glDepthFunc(GL_LESS);
}

void SkyboxManager::cleanup() {
    if (skyboxVAO) {
        glDeleteVertexArrays(1, &skyboxVAO);
        skyboxVAO = 0;
    }
    if (skyboxVBO) {
        glDeleteBuffers(1, &skyboxVBO);
        skyboxVBO = 0;
    }
    if (skyboxTexture) {
        glDeleteTextures(1, &skyboxTexture);
        skyboxTexture = 0;
    }
    initialized = false;
}

unsigned int SkyboxManager::createNightSkyCubemap() {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width = 512, height = 512;
    
    for (unsigned int i = 0; i < 6; i++) {
        unsigned char* data = new unsigned char[width * height * 3];
        
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int idx = (y * width + x) * 3;
                
                // Convert to normalized coordinates [-1, 1]
                float u = (2.0f * x / width) - 1.0f;
                float v = (2.0f * y / height) - 1.0f;
                
                // Calculate direction vector based on face
                Vector3f dir;
                switch (i) {
                    case 0: dir = Vector3f(1.0f, -v, -u); break;  // +X
                    case 1: dir = Vector3f(-1.0f, -v, u); break;  // -X  
                    case 2: dir = Vector3f(u, 1.0f, v); break;    // +Y
                    case 3: dir = Vector3f(u, -1.0f, -v); break;  // -Y
                    case 4: dir = Vector3f(u, -v, 1.0f); break;   // +Z
                    case 5: dir = Vector3f(-u, -v, -1.0f); break; // -Z
                }
                dir.normalize();
                
                // Night sky gradient - darker at horizon, lighter at zenith
                float altitude = dir.y;
                float horizonMask = std::max(0.0f, altitude);
                
                // Base night sky colors
                Vector3f zenithColor(0.12f, 0.12f, 0.25f);   // Brighter dark blue
                Vector3f horizonColor(0.06f, 0.06f, 0.18f);  // Brighter very dark blue
                
                // Interpolate between horizon and zenith
                Vector3f skyColor = horizonColor * (1.0f - horizonMask) + zenithColor * horizonMask;
                
                // Add some stars
                float starNoise = (std::sin(dir.x * 123.456f) * std::cos(dir.y * 234.567f) * std::sin(dir.z * 345.678f));
                if (starNoise > 0.992f) {
                    skyColor = skyColor + Vector3f(1.2f, 1.2f, 1.4f);
                }
                
                // Add subtle purple tint for spooky atmosphere
                skyColor.x += 0.03f;
                skyColor.z += 0.05f;
                
                // Clamp values
                skyColor.x = std::min(1.0f, std::max(0.0f, skyColor.x));
                skyColor.y = std::min(1.0f, std::max(0.0f, skyColor.y));
                skyColor.z = std::min(1.0f, std::max(0.0f, skyColor.z));
                
                data[idx] = (unsigned char)(skyColor.x * 255);
                data[idx + 1] = (unsigned char)(skyColor.y * 255);
                data[idx + 2] = (unsigned char)(skyColor.z * 255);
            }
        }
        
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        delete[] data;
    }
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    return textureID;
}