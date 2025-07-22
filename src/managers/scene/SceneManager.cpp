#include "SceneManager.h"
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../../../include/rendering/ShaderManager.h"
#include "../../../include/math/Matrix4f.h"

SceneManager::SceneManager() : initialized(false) {
    skyboxVAO = skyboxVBO = skyboxTexture = 0;
    mirrorVAO = mirrorVBO = mirrorEBO = dirtyMirrorTexture = 0;
}

SceneManager::~SceneManager() {
    cleanup();
}

bool SceneManager::initialize(int windowWidth, int windowHeight, const std::string& title) {
    std::cout << "=== Initializing Luigi's Mansion Hall ===" << std::endl;
    
    // Initialize camera first
    camera = std::make_unique<Camera>(60.0f, 100.0f, 0.1f, 
        Vector3f(0.0f, 2.0f, 16.75f), Vector3f(0.0f, 0.0f, -1.0f));
    camera->setup(windowWidth, windowHeight);
    
    // Initialize all managers
    if (!initializeManagers()) {
        std::cerr << "Failed to initialize managers" << std::endl;
        return false;
    }
    
    // Load resources
    if (!loadResources()) {
        std::cerr << "Failed to load resources" << std::endl;
        return false;
    }
    
    // Setup scene components
    setupScene();
    
    initialized = true;
    std::cout << "=== Luigi's Mansion Hall initialized successfully ===" << std::endl;
    return true;
}

bool SceneManager::initializeManagers() {
    // Create window manager
    windowManager = std::make_unique<WindowManager>(1920, 1080, "Luigi's Mansion Hall");
    if (!windowManager->initialize()) {
        return false;
    }
    
    // Create input manager
    inputManager = std::make_unique<InputManager>();
    inputManager->initialize(camera.get(), windowManager->getWidth(), windowManager->getHeight());
    
    // Setup callbacks
    windowManager->setupCallbacks(camera.get(), 
        inputManager->getKeysArray(),
        inputManager->getPausedFlag(),
        inputManager->getFirstMouseFlag(),
        inputManager->getLastXPos(),
        inputManager->getLastYPos(),
        inputManager->getMouseSensitivity());
    
    // Create model manager
    modelManager = std::make_unique<ModelManager>();
    
    // Create light manager
    lightManager = std::make_unique<LightManager>();
    
    return true;
}

bool SceneManager::loadResources() {
    // Initialize shader manager
    ShaderManager::initialize();
    
    // Load lighting configuration
    std::string configPath = "config.xml";
    if (!lightManager->loadFromXML(configPath)) {
        std::cerr << "Warning: Failed to load lights from XML, using defaults" << std::endl;
    }
    
    // Load models
    if (!modelManager->loadAllModels()) {
        std::cerr << "Warning: Some models failed to load" << std::endl;
    }
    
    return true;
}

void SceneManager::setupScene() {
    // Setup skybox
    setupSkybox();
    
    // Setup mirror/glass doorway
    setupMirror();
}

void SceneManager::setupSkybox() {
    // Create night sky cubemap
    skyboxTexture = createNightSkyCubemap();
    
    // Skybox vertices for a large fixed cube around the house (scale 100x)
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
}

void SceneManager::setupMirror() {
    // Setup dirty glass texture
    dirtyMirrorTexture = generateDirtyMirrorTexture();
    
    // Mirror vertices
    float mirrorVertices[] = {
        -4.0f, -7.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
         4.0f, -7.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
         4.0f,  7.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
        -4.0f,  7.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f
    };

    unsigned int mirrorIndices[] = {
        0, 1, 2,
        0, 2, 3
    };

    glGenVertexArrays(1, &mirrorVAO);
    glGenBuffers(1, &mirrorVBO);
    glGenBuffers(1, &mirrorEBO);

    glBindVertexArray(mirrorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mirrorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mirrorVertices), mirrorVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mirrorEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(mirrorIndices), mirrorIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void SceneManager::run() {
    if (!initialized) {
        std::cerr << "SceneManager not initialized!" << std::endl;
        return;
    }
    
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    // Main render loop
    while (!windowManager->shouldClose()) {
        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Process input
        inputManager->processInput(deltaTime);
        inputManager->updateEffects(deltaTime);
        inputManager->updateHUD(deltaTime);

        // Clear screen
        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update camera
        camera->updateCamera();
        
        // Render scene
        renderScene();

        // Swap buffers and poll events
        windowManager->swapBuffers();
        windowManager->pollEvents();
    }
}

void SceneManager::renderScene() {
    // Render skybox first
    renderSkybox();
    
    // Setup uniforms for all shaders
    setupCommonUniforms(ShaderManager::getShaderForMaterial(MaterialType::PHONG));
    setupCommonUniforms(ShaderManager::getShaderForMaterial(MaterialType::LAMBERT));
    
    // Render models
    modelManager->renderAllModels();
    
    // Render Boo if available
    renderBoo();
    
    // Render mirror
    renderMirror();
}

void SceneManager::setupCommonUniforms(class Shader* shader) {
    if (!shader) return;
    
    shader->use();
    shader->setVec3("viewPosition", camera->position.x, camera->position.y, camera->position.z);
    
    // Apply lights from LightManager
    lightManager->applyToShader(shader, camera->position, inputManager->getBreathingTime());
    
    // IBL - Bind skybox for Image Based Lighting
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
    shader->setInt("skybox", 1);
    
    // Matrix uniforms
    Matrix4f model;
    model.a11 = 0.1f; model.a22 = 0.1f; model.a33 = 0.1f; model.a44 = 1.0f;
    model.a41 = 0.0f; model.a42 = 0.0f; model.a43 = 14.0f;
    model.transpose();
    shader->setMat4("model", &model.a11);
    
    Matrix4f viewT = camera->viewMatrix;
    shader->setMat4("view", &viewT.a11);
    
    Matrix4f projT = camera->projectionMatrix;
    projT.transpose();
    shader->setMat4("projection", &projT.a11);
}

void SceneManager::renderSkybox() {
    // Render skybox as a fixed dome around the house
    glDepthFunc(GL_LEQUAL);
    
    Shader* skyboxShader = ShaderManager::getShaderForLight(LightType::SKYBOX);
    if (skyboxShader) {
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
    }
    
    glDepthFunc(GL_LESS);
}

void SceneManager::renderBoo() {
    Model* booModel = modelManager->getBooModel();
    if (!booModel) return;
    
    Shader* booShader = ShaderManager::getShaderForMaterial(MaterialType::LAMBERT);
    if (!booShader) return;
    
    booShader->use();
    
    // Calculate Boo position
    float booZ = -camera->position.z;
    Vector3f booPosition(camera->position.x, camera->position.y, booZ);
    
    // Setup uniforms with Boo's light
    booShader->setVec3("viewPosition", camera->position.x, camera->position.y, camera->position.z);
    lightManager->applyToShaderWithBoo(booShader, camera->position, booPosition, inputManager->getBreathingTime());
    
    // IBL
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
    booShader->setInt("skybox", 1);
    
    // Set view and projection matrices
    Matrix4f viewT = camera->viewMatrix;
    booShader->setMat4("view", &viewT.a11);
    
    Matrix4f projT = camera->projectionMatrix;
    projT.transpose();
    booShader->setMat4("projection", &projT.a11);

    // Boo model matrix with rotation
    Matrix4f booModelMatrix;
    booModelMatrix.a11 = 5.0f;
    booModelMatrix.a22 = 5.0f;
    booModelMatrix.a33 = 5.0f;
    booModelMatrix.a44 = 1.0f;

    // Simple rotation using camera angles
    float yawRad = (-camera->yAngle - 90.0f) * 3.14159f / 180.0f;
    float pitchRad = camera->xAngle * 3.14159f / 180.0f;

    // Yaw rotation (Y axis)
    Matrix4f yawMatrix;
    yawMatrix.a11 = cos(yawRad); yawMatrix.a13 = sin(yawRad);
    yawMatrix.a31 = -sin(yawRad); yawMatrix.a33 = cos(yawRad);

    // Pitch rotation (X axis)
    Matrix4f pitchMatrix;
    pitchMatrix.a22 = cos(pitchRad); pitchMatrix.a23 = -sin(pitchRad);
    pitchMatrix.a32 = sin(pitchRad); pitchMatrix.a33 = cos(pitchRad);

    // Scale matrix
    Matrix4f scaleMatrix;
    scaleMatrix.a11 = 0.07f; scaleMatrix.a22 = 0.07f; scaleMatrix.a33 = 0.07f;

    // Combine: Scale * Yaw * Pitch
    booModelMatrix = scaleMatrix * yawMatrix * pitchMatrix;

    booModelMatrix.a41 = camera->position.x;
    booModelMatrix.a42 = camera->position.y;
    booModelMatrix.a43 = booZ;

    booModelMatrix.transpose();
    booShader->setMat4("model", &booModelMatrix.a11);
    
    booModel->DrawWithMaterials();
}

void SceneManager::renderMirror() {
    // Render dirty glass doorway
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader* mirrorShader = ShaderManager::getMirrorShader();
    if (mirrorShader) {
        mirrorShader->use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, dirtyMirrorTexture);
        mirrorShader->setInt("dirtyTexture", 0);

        mirrorShader->setVec3("viewPos", camera->position.x, camera->position.y, camera->position.z);
        mirrorShader->setFloat("time", (float)glfwGetTime());
        mirrorShader->setFloat("ghostEffect", 1.0f);

        Matrix4f mirrorModel;
        mirrorShader->setMat4("model", &mirrorModel.a11);

        Matrix4f viewT = camera->viewMatrix;
        mirrorShader->setMat4("view", &viewT.a11);

        Matrix4f projT = camera->projectionMatrix;
        projT.transpose();
        mirrorShader->setMat4("projection", &projT.a11);

        glBindVertexArray(mirrorVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    glDisable(GL_BLEND);
}

unsigned int SceneManager::generateDirtyMirrorTexture() {
    const int texSize = 512;
    unsigned char* data = new unsigned char[texSize * texSize * 4];

    for (int y = 0; y < texSize; y++) {
        for (int x = 0; x < texSize; x++) {
            int idx = (y * texSize + x) * 4;

            float baseClean = 0.9f;

            float centerX = texSize * 0.5f;
            float centerY = texSize * 0.5f;
            float distFromCenter = sqrtf((x - centerX) * (x - centerX) + (y - centerY) * (y - centerY));
            float radiusEffect = 1.0f - (distFromCenter / (texSize * 0.7f));

            float noise = (rand() % 100) / 100.0f;
            float dirtSpots = (noise > 0.85f) ? 0.3f : 1.0f;

            float smearX = sinf(y * 0.03f) * cosf(x * 0.02f) * 0.4f + 0.6f;
            float smearY = sinf(x * 0.025f) * cosf(y * 0.035f) * 0.3f + 0.7f;

            float edgeDirt = 1.0f;
            if (x < 20 || x > texSize - 20 || y < 20 || y > texSize - 20) {
                edgeDirt = 0.5f;
            }

            float waterStains = 1.0f;
            if ((x + (int)(sinf(y * 0.1f) * 5)) % 47 < 2) {
                waterStains = 0.6f;
            }

            float finalDirt = baseClean * dirtSpots * smearX * smearY * edgeDirt * waterStains;
            finalDirt = finalDirt * radiusEffect + (1.0f - radiusEffect) * 0.6f;
            finalDirt = std::max(0.7f, std::min(1.0f, finalDirt));

            data[idx] = (unsigned char)(finalDirt * 255);
            data[idx + 1] = (unsigned char)(finalDirt * 255);
            data[idx + 2] = (unsigned char)(finalDirt * 255);
            data[idx + 3] = 255;
        }
    }

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSize, texSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    delete[] data;
    return texture;
}

unsigned int SceneManager::createNightSkyCubemap() {
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
                
                // Night sky gradient
                float altitude = dir.y;
                float horizonMask = std::max(0.0f, altitude);
                
                // Base night sky colors
                Vector3f zenithColor(0.12f, 0.12f, 0.25f);
                Vector3f horizonColor(0.06f, 0.06f, 0.18f);
                
                // Interpolate between horizon and zenith
                Vector3f skyColor = horizonColor * (1.0f - horizonMask) + zenithColor * horizonMask;
                
                // Add some stars
                float starNoise = (std::sin(dir.x * 123.456f) * std::cos(dir.y * 234.567f) * std::sin(dir.z * 345.678f));
                if (starNoise > 0.992f) {
                    skyColor = skyColor + Vector3f(1.2f, 1.2f, 1.4f);
                }
                
                // Add subtle purple tint
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

void SceneManager::cleanup() {
    // Cleanup OpenGL resources
    if (skyboxVAO) {
        glDeleteVertexArrays(1, &skyboxVAO);
        glDeleteBuffers(1, &skyboxVBO);
        glDeleteTextures(1, &skyboxTexture);
        skyboxVAO = skyboxVBO = skyboxTexture = 0;
    }
    
    if (mirrorVAO) {
        glDeleteVertexArrays(1, &mirrorVAO);
        glDeleteBuffers(1, &mirrorVBO);
        glDeleteBuffers(1, &mirrorEBO);
        glDeleteTextures(1, &dirtyMirrorTexture);
        mirrorVAO = mirrorVBO = mirrorEBO = dirtyMirrorTexture = 0;
    }
    
    // Cleanup shader manager
    ShaderManager::cleanup();
}