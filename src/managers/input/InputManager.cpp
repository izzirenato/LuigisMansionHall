#include "InputManager.h"
#include <iostream>
#include <cmath>
#include <iomanip>

const float InputManager::HUD_UPDATE_INTERVAL = 0.1f;

InputManager::InputManager() 
    : isPaused(false), firstMouse(true), mouseSensitivity(0.1f),
      breathingTime(0.0f), breathingIntensity(0.08f), breathingSpeed(1.2f),
      hudUpdateTimer(0.0f), camera(nullptr) {
    
    // Initialize keys array
    for (int i = 0; i < 1024; i++) {
        keys[i] = false;
    }
}

InputManager::~InputManager() {
}

void InputManager::initialize(Camera* cam, int windowWidth, int windowHeight) {
    camera = cam;
    lastX = windowWidth * 0.5;
    lastY = windowHeight * 0.5;
    
    std::cout << "=== Luigi's Mansion Hall - Controls ===" << std::endl;
    std::cout << "WASD - Move around" << std::endl;
    std::cout << "Space - Move up" << std::endl;
    std::cout << "Shift - Move down" << std::endl;
    std::cout << "Mouse - Look around" << std::endl;
    std::cout << "P - Pause/Resume" << std::endl;
    std::cout << "ESC - Exit" << std::endl;
    std::cout << "=======================================" << std::endl;
}

void InputManager::processInput(float deltaTime) {
    if (!camera || isPaused) return;

    if (keys[GLFW_KEY_W]) camera->move('W', deltaTime);
    if (keys[GLFW_KEY_S]) camera->move('S', deltaTime);
    if (keys[GLFW_KEY_A]) camera->move('A', deltaTime);
    if (keys[GLFW_KEY_D]) camera->move('D', deltaTime);
    if (keys[GLFW_KEY_SPACE]) camera->move('U', deltaTime);
    if (keys[GLFW_KEY_LEFT_SHIFT]) camera->move('N', deltaTime);
}

void InputManager::updateBreathingEffect(float deltaTime) {
    if (!camera || isPaused) return;

    breathingTime += deltaTime;

    float breathingY = sinf(breathingTime * breathingSpeed) * breathingIntensity;
    float breathingX = cosf(breathingTime * breathingSpeed * 0.7f) * breathingIntensity * 0.3f;

    camera->position.y += breathingY * deltaTime;
    camera->position.x += breathingX * deltaTime;

    bool isMoving = keys[GLFW_KEY_W] || keys[GLFW_KEY_S] || keys[GLFW_KEY_A] || keys[GLFW_KEY_D];
    if (isMoving) {
        float walkBobY = sinf(breathingTime * 6.0f) * 0.025f;
        float walkBobX = cosf(breathingTime * 3.0f) * 0.015f;
        camera->position.y += walkBobY * deltaTime;
        camera->position.x += walkBobX * deltaTime;
    }
}

void InputManager::displayCameraHUD(float deltaTime) {
    static float timer = 0.0f;
    timer += deltaTime;

    if (timer >= HUD_UPDATE_INTERVAL) {
        timer = 0.0f;

#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif

        std::cout << "=== LUIGI'S MANSION HALL - CAMERA HUD ===" << std::endl;
        std::cout << "Position: X=" << std::fixed << std::setprecision(2)
            << camera->position.x << " Y=" << camera->position.y
            << " Z=" << camera->position.z << std::endl;
        std::cout << "Rotation: Yaw=" << camera->yAngle
            << "° Pitch=" << camera->xAngle << "°" << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        std::cout << "Controls: WASD=Move, Space/Shift=Up/Down" << std::endl;
        std::cout << "P=Pause, ESC=Exit" << std::endl;
        std::cout << "=========================================" << std::endl;
    }
}

void InputManager::updateEffects(float deltaTime) {
    updateBreathingEffect(deltaTime);
}

void InputManager::updateHUD(float deltaTime) {
    if (camera && !isPaused) {
        displayCameraHUD(deltaTime);
    }
}