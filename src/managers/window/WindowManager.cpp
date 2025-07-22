#include "WindowManager.h"
#include <iostream>
#include "../../../include/math/Camera.h"

// Static member definitions
Camera* WindowManager::camera = nullptr;
bool* WindowManager::keys = nullptr;
bool* WindowManager::isPaused = nullptr;
bool* WindowManager::firstMouse = nullptr;
double* WindowManager::lastX = nullptr;
double* WindowManager::lastY = nullptr;
float* WindowManager::mouseSensitivity = nullptr;

WindowManager::WindowManager(int w, int h, const std::string& title) 
    : window(nullptr), width(w), height(h), title(title) {
}

WindowManager::~WindowManager() {
    cleanup();
}

bool WindowManager::initialize() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    
    // Set OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Create window
    window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    
    // Make context current
    glfwMakeContextCurrent(window);
    
    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    
    // Set up OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_CULL_FACE);
    glViewport(0, 0, width, height);
    
    std::cout << "Window initialized successfully: " << width << "x" << height << std::endl;
    return true;
}

void WindowManager::setupCallbacks(Camera* cam, bool* keyArray, bool* paused, bool* firstMouseFlag, 
                                  double* lastXPos, double* lastYPos, float* mouseSens) {
    // Store pointers for callbacks
    camera = cam;
    keys = keyArray;
    isPaused = paused;
    firstMouse = firstMouseFlag;
    lastX = lastXPos;
    lastY = lastYPos;
    mouseSensitivity = mouseSens;
    
    // Set GLFW callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void WindowManager::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    if (camera) camera->setup(width, height);
}

void WindowManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        *isPaused = !(*isPaused);
        if (*isPaused) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            std::cout << "Game paused. Press P to resume." << std::endl;
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            *firstMouse = true;
            std::cout << "Game resumed." << std::endl;
        }
    }

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) keys[key] = true;
        else if (action == GLFW_RELEASE) keys[key] = false;
    }
}

void WindowManager::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (*isPaused) return;

    if (*firstMouse) {
        *lastX = xpos;
        *lastY = ypos;
        *firstMouse = false;
    }

    double xoffset = xpos - *lastX;
    double yoffset = *lastY - ypos;
    *lastX = xpos;
    *lastY = ypos;

    xoffset *= *mouseSensitivity;
    yoffset *= *mouseSensitivity;

    if (camera) {
        camera->rotationY((float)xoffset, 1.0f);
        camera->rotationX((float)yoffset, 1.0f);
    }
}

bool WindowManager::shouldClose() const {
    return glfwWindowShouldClose(window);
}

void WindowManager::swapBuffers() {
    glfwSwapBuffers(window);
}

void WindowManager::pollEvents() {
    glfwPollEvents();
}

void WindowManager::close() {
    glfwSetWindowShouldClose(window, GL_TRUE);
}

void WindowManager::cleanup() {
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}