#ifndef EFFECTS_MANAGER_H
#define EFFECTS_MANAGER_H

#include <glad/glad.h>
#include "../../include/math/Camera.h"
#include "../../include/rendering/Shader.h"

class EffectsManager {
private:
    // Mirror effect variables
    unsigned int mirrorVAO, mirrorVBO, mirrorEBO;
    unsigned int dirtyMirrorTexture;
    bool initialized;
    
    // Generate procedural dirty mirror texture
    unsigned int generateDirtyMirrorTexture();
    
    // Setup mirror geometry
    void setupMirrorGeometry();
    
public:
    EffectsManager();
    ~EffectsManager();
    
    // Initialize all effects
    bool initialize();
    
    // Render dirty glass doorway effect
    void renderDirtyMirror(Camera* camera);
    
    // Cleanup
    void cleanup();
    
    // Getters
    bool isInitialized() const { return initialized; }
};

#endif // EFFECTS_MANAGER_H