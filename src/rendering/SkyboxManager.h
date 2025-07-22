#ifndef SKYBOX_MANAGER_H
#define SKYBOX_MANAGER_H

#include <glad/glad.h>
#include "../../include/math/Vector3f.h"
#include "../../include/math/Matrix4f.h"
#include "../../include/math/Camera.h"
#include "../../include/rendering/Shader.h"

class SkyboxManager {
private:
    unsigned int skyboxVAO, skyboxVBO;
    unsigned int skyboxTexture;
    
    bool initialized;
    
    // Generate procedural night sky cubemap
    unsigned int createNightSkyCubemap();
    
public:
    SkyboxManager();
    ~SkyboxManager();
    
    // Initialize skybox
    bool initialize();
    
    // Render skybox
    void render(Camera* camera);
    
    // Getters
    unsigned int getSkyboxTexture() const { return skyboxTexture; }
    bool isInitialized() const { return initialized; }
    
    // Cleanup
    void cleanup();
};

#endif // SKYBOX_MANAGER_H