#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <string>
#include <memory>
#include "../../../include/math/Camera.h"
#include "../window/WindowManager.h"
#include "../input/InputManager.h"
#include "../models/ModelManager.h"
#include "../lighting/LightManager.h"

class SceneManager {
private:
    // Core components
    std::unique_ptr<WindowManager> windowManager;
    std::unique_ptr<InputManager> inputManager;
    std::unique_ptr<ModelManager> modelManager;
    std::unique_ptr<LightManager> lightManager;
    
    // Camera
    std::unique_ptr<Camera> camera;
    
    // Scene state
    bool initialized;
    
    // Skybox
    unsigned int skyboxVAO, skyboxVBO;
    unsigned int skyboxTexture;
    
    // Mirror/glass doorway
    unsigned int mirrorVAO, mirrorVBO, mirrorEBO;
    unsigned int dirtyMirrorTexture;
    
    // Scene initialization functions
    bool initializeManagers();
    bool loadResources();
    void setupSkybox();
    void setupMirror();
    void setupScene();
    
    // Scene rendering functions
    void renderSkybox();
    void renderMirror();
    void renderBoo();
    void renderScene();
    
    // Cleanup functions
    void cleanup();
    
    // Helper functions
    unsigned int generateDirtyMirrorTexture();
    unsigned int createNightSkyCubemap();
    void setupCommonUniforms(class Shader* shader);
    
public:
    SceneManager();
    ~SceneManager();
    
    // Initialize the entire scene
    bool initialize(int windowWidth = 1920, int windowHeight = 1080, 
                   const std::string& title = "Luigi's Mansion Hall");
    
    // Main game loop
    void run();
    
    // Getters
    bool isInitialized() const { return initialized; }
    WindowManager* getWindowManager() const { return windowManager.get(); }
    InputManager* getInputManager() const { return inputManager.get(); }
    ModelManager* getModelManager() const { return modelManager.get(); }
    LightManager* getLightManager() const { return lightManager.get(); }
    Camera* getCamera() const { return camera.get(); }
};

#endif // SCENE_MANAGER_H