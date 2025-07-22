#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

class Camera;

class WindowManager {
private:
    GLFWwindow* window;
    int width, height;
    std::string title;
    
    // Callback functions
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    
    // Static pointers for callbacks
    static Camera* camera;
    static bool* keys;
    static bool* isPaused;
    static bool* firstMouse;
    static double* lastX;
    static double* lastY;
    static float* mouseSensitivity;
    
public:
    WindowManager(int w, int h, const std::string& title);
    ~WindowManager();
    
    // Initialize GLFW and create window
    bool initialize();
    
    // Setup callbacks with external variables
    void setupCallbacks(Camera* cam, bool* keyArray, bool* paused, bool* firstMouseFlag, 
                       double* lastXPos, double* lastYPos, float* mouseSens);
    
    // Window management
    bool shouldClose() const;
    void swapBuffers();
    void pollEvents();
    void close();
    
    // Getters
    GLFWwindow* getWindow() const { return window; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    
    // Cleanup
    void cleanup();
};

#endif // WINDOW_MANAGER_H