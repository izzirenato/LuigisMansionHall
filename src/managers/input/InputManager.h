#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <GLFW/glfw3.h>
#include "../../../include/math/Camera.h"

class InputManager {
private:
    bool keys[1024];
    bool isPaused;
    bool firstMouse;
    double lastX, lastY;
    float mouseSensitivity;
    
    // Camera movement and effects
    float breathingTime;
    float breathingIntensity;
    float breathingSpeed;
    
    // HUD display
    float hudUpdateTimer;
    static const float HUD_UPDATE_INTERVAL;
    
    Camera* camera;
    
    // Helper functions
    void updateBreathingEffect(float deltaTime);
    void displayCameraHUD(float deltaTime);
    
public:
    InputManager();
    ~InputManager();
    
    // Initialize with camera reference
    void initialize(Camera* cam, int windowWidth, int windowHeight);
    
    // Process input for current frame
    void processInput(float deltaTime);
    
    // Update breathing effects
    void updateEffects(float deltaTime);
    
    // Display HUD if enabled
    void updateHUD(float deltaTime);
    
    // Getters for window manager
    bool* getKeysArray() { return keys; }
    bool* getPausedFlag() { return &isPaused; }
    bool* getFirstMouseFlag() { return &firstMouse; }
    double* getLastXPos() { return &lastX; }
    double* getLastYPos() { return &lastY; }
    float* getMouseSensitivity() { return &mouseSensitivity; }
    
    // Getters for game state
    bool isGamePaused() const { return isPaused; }
    float getBreathingTime() const { return breathingTime; }
    
    // Settings
    void setMouseSensitivity(float sensitivity) { mouseSensitivity = sensitivity; }
    void setBreathingSettings(float intensity, float speed) { 
        breathingIntensity = intensity; 
        breathingSpeed = speed; 
    }
};

#endif // INPUT_MANAGER_H