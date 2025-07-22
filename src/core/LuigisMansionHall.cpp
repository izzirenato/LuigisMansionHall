#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <filesystem>                   // Abbiamo utilizzato questa libreria per  la singola funzione setprecision che usiamo per l'output della camera
#include "math/Matrix3f.h"
#include "math/Matrix4f.h"
#include "math/Vector3f.h"
#include "math/Vector4f.h"
#include "math/Quaternion.h"
#include "math/Camera.h"
#include "rendering/Model.h"
#include "rendering/Shader.h"
#include "rendering/ShaderManager.h"
#include "../managers/lighting/LightManager.h"
#include "../managers/models/ModelManager.h"
#include "../rendering/SkyboxManager.h"
#include "../effects/EffectsManager.h"

namespace fs = std::filesystem;

// Variabili globali 
Camera* camera = nullptr;
LightManager* lightManager = nullptr;
SkyboxManager* skyboxManager = nullptr;
ModelManager* modelManager = nullptr;
EffectsManager* effectsManager = nullptr;

int w = 1920, h = 1080;                     //Dimensione dello schermo
bool keys[1024];                            // Array per gestire gli input da tastiera
double lastX = w * 0.5, lastY = h * 0.5;    // Posizione del mouse per inizializzarlo al centro dello schermo
bool firstMouse = true;
bool isPaused = false;
float mouseSensitivity = 0.1f;
const float HUD_UPDATE_INTERVAL = 0.5f;     // Intervallo di aggiornamento dell'HUD della camera


// effetto di respirazione boo 
float breathingTime = 0.0f;
float breathingIntensity = 0.08f;
float breathingSpeed = 1.2f;


// Variabile per l'effetto di bioluminescenza
float biolumTime = 0.0f;
float biolumIntensity = 1.2f;
float biolumSpeed = 2.0f;

// Firma delle funzioni
void setupCommonUniforms(Shader* shader);
void renderModels();


// Funzione che gestisce il ridimensionamento della finestra
void framebuffer_size_callback(GLFWwindow* window, int width, int height) 
{
    glViewport(0, 0, width, height);
    if (camera) camera->setup(width, height);
}


// Funzione che gestiscegli input da tastiera
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	// Uscita dall 'applicazione con il tasto ESC
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    // Pausa dell'applicazione
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        isPaused = !isPaused;
        if (isPaused) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            std::cout << "Scene in pausa, premi per P per togliere la pausa." << std::endl;
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            firstMouse = true;
            std::cout << "Scena avviata." << std::endl;
        }
    }
	// Gestione degli input da tastiera per il movimento della camera
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) keys[key] = true;
        else if (action == GLFW_RELEASE) keys[key] = false;
    }
}


// Funzione che gestisce gli input del mouse per la rotazione della camera
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (isPaused) return;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    if (camera) {
        camera->rotationY((float)xoffset, 1.0f);    // Rotazione rispetto all'asse Y Yaw
		camera->rotationX((float)yoffset, 1.0f);    // Rotazione rispetto all'asse X Pitch
    }
}


// Funzione dell'effetto di respirazione della camera
void updateBreathingEffect(float deltaTime) {
    if (!camera || isPaused) return;

	breathingTime += deltaTime;                     // DeltaTime è il tempo trascorso dall'ultimo frame

    float breathingY = sinf(breathingTime * breathingSpeed) * breathingIntensity;
    float breathingX = cosf(breathingTime * breathingSpeed * 0.7f) * breathingIntensity * 0.3f;

    camera->position.y += breathingY * deltaTime;
    camera->position.x += breathingX * deltaTime;

    bool isMoving = keys[GLFW_KEY_W] || keys[GLFW_KEY_S] || keys[GLFW_KEY_A] || keys[GLFW_KEY_D];
	if (isMoving) {                                 // Se la camera si sta muovendo, aggiungi l'effetto di bobbing
        float walkBobY = sinf(breathingTime * 6.0f) * 0.025f;
        float walkBobX = cosf(breathingTime * 3.0f) * 0.015f;
        camera->position.y += walkBobY * deltaTime;
        camera->position.x += walkBobX * deltaTime;
    }
}


// Funzione per processare gli input che muoveranno la camera della camera
void processInput(float deltaTime) {
    if (!camera || isPaused) return;

    if (keys[GLFW_KEY_W]) camera->move('W', deltaTime);
    if (keys[GLFW_KEY_S]) camera->move('S', deltaTime);
    if (keys[GLFW_KEY_A]) camera->move('A', deltaTime);
    if (keys[GLFW_KEY_D]) camera->move('D', deltaTime);
    if (keys[GLFW_KEY_SPACE]) camera->move('U', deltaTime);
    if (keys[GLFW_KEY_LEFT_SHIFT]) camera->move('N', deltaTime);

    updateBreathingEffect(deltaTime);
}
//  Funzione per visualizzare le coordinate di camera in console
void displayCameraHUD(Camera* camera, float deltaTime) {
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


// Funzione per impostare i CommonUniforms per tutti gli shader
void setupCommonUniforms(Shader* shader) {

    // Le Shader devono sapere dove si trova la camera
    shader->setVec3("viewPosition", camera->position.x, camera->position.y, camera->position.z);
    
	// e dove si trovano le luci
    if (lightManager) {
        lightManager->applyToShader(shader, camera->position, breathingTime);
    }
    
	// IBL Image Based Lighting della skybox 
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxManager->getSkyboxTexture());
    shader->setInt("skybox", 1);
    
	// Model Matrix
    Matrix4f model;
    model.a11 = 0.1f; model.a22 = 0.1f; model.a33 = 0.1f; model.a44 = 1.0f;
    model.a41 = 0.0f; model.a42 = 0.0f; model.a43 = 14.0f;
    model.transpose();
    shader->setMat4("model", &model.a11);

	// View Matrix
    Matrix4f viewT = camera->viewMatrix;
    shader->setMat4("view", &viewT.a11);

	// Projection Matrix
    Matrix4f projT = camera->projectionMatrix;
    projT.transpose();
    shader->setMat4("projection", &projT.a11);
}


// Funzione per renderizzare tutti i modelli della scena
void renderModels() {
    if (modelManager) {
        modelManager->renderAllModels();
    }
}


// Funzione Main dell'applicazione
int main() {
	// Inizializzazione di GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(w, h, "Luigi's Mansion Hall", NULL, NULL);
    if (!window) {
        std::cout << "La finestra GLFW non è stata creata" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Fallita la inizializzazione di GLAD" << std::endl;
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	// Inizializzazione di OpenGL
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_CULL_FACE);
    glViewport(0, 0, w, h);

	// Inizializzazione della camera (angolo/far/near/pos/lookAt)
    camera = new Camera(60.0f, 100.0f, 0.1f, Vector3f(0.0f, 3.5f, 16.75f), Vector3f(0.0f, 0.0f, -1.0f));
    camera->setup(w, h);

    // Inizializzazione del model manager e caricamento di tutti i modelli
    modelManager = new ModelManager();
    if (!modelManager->loadAllModels()) {
        std::cerr << "Warning: Almeno un modello non si è caricato" << std::endl;
    }

    // Inizializzazione dell'effects manager
    effectsManager = new EffectsManager();
    if (!effectsManager->initialize()) {
        std::cerr << "Warning: l'effectManager non è stato inizializzato correttamente" << std::endl;
    }

    // Inizializzazione dello shader manager
    ShaderManager::initialize();

    // Inizializzazione del light manager e caricamento delle luci da XML
    lightManager = new LightManager();
    std::string configPath = "config.xml";
    if (!lightManager->loadFromXML(configPath)) {
        std::cerr << "Warning: Fallimento nella lettura luci da XML" << std::endl;
    }

    // Inizializzazione del skybox manager
    skyboxManager = new SkyboxManager();
    if (!skyboxManager->initialize()) {
        std::cerr << "Warning: Fallimento inizializzazione skybox manager" << std::endl;
    }


	// Comandi dell'applicazione
    std::cout << "=== Luigi's Mansion Hall ===" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "WASD - Move around" << std::endl;
    std::cout << "Space - Move up" << std::endl;
    std::cout << "Shift - Move down" << std::endl;
    std::cout << "Mouse - Look around" << std::endl;
    std::cout << "P - Pause/Resume" << std::endl;
    std::cout << "ESC - Exit" << std::endl;
    std::cout << "=============================" << std::endl;

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;


    // Loop principale di rendering
    while (!glfwWindowShouldClose(window)) {
        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(deltaTime);

        // Aggiornamento del tempo per l'effetto di bioluminescenza
        biolumTime += deltaTime;

        if (camera && !isPaused) {
            displayCameraHUD(camera, deltaTime);
        }

        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera->updateCamera();

        // Renderizzazione della skybox per prima
        skyboxManager->render(camera);

        // === SETUP DEGLI UNIFORM PER TUTTI GLI SHADER PRIMA DEL RENDERING ===
        // Configurazione delle luci e matrici per entrambi i material shader

        // Setup degli uniform per il Phong shader
        Shader* phongShader = ShaderManager::getShaderForMaterial(MaterialType::PHONG);
        if (phongShader) {
            phongShader->use();
            setupCommonUniforms(phongShader);

            Matrix4f roomMatrix;
            roomMatrix.a11 = 0.1f; roomMatrix.a22 = 0.1f; roomMatrix.a33 = 0.1f; roomMatrix.a44 = 1.0f;
            roomMatrix.a41 = 0.0f; roomMatrix.a42 = 0.0f; roomMatrix.a43 = 14.0f;
            roomMatrix.transpose();
            phongShader->setMat4("model", &roomMatrix.a11);
        }

        // Setup degli uniform per il Lambert shader
        Shader* lambertShader = ShaderManager::getShaderForMaterial(MaterialType::LAMBERT);
        if (lambertShader) {
            lambertShader->use();
            setupCommonUniforms(lambertShader);

            Matrix4f roomMatrix;
            roomMatrix.a11 = 0.1f; roomMatrix.a22 = 0.1f; roomMatrix.a33 = 0.1f; roomMatrix.a44 = 1.0f;
            roomMatrix.a41 = 0.0f; roomMatrix.a42 = 0.0f; roomMatrix.a43 = 14.0f;
            roomMatrix.transpose();
            lambertShader->setMat4("model", &roomMatrix.a11);
        }

        // Ora renderizza i modelli - DrawWithMaterials cambierà shader ma gli uniform sono già impostati
        renderModels();

        // Disegna il modello Boo con la luce respiratoria
        Model* booModel = modelManager ? modelManager->getBooModel() : nullptr;
        if (booModel) {
            Shader* booShader = ShaderManager::getShaderForMaterial(MaterialType::LAMBERT);
            if (booShader) {
                booShader->use();

                // Calcola prima la posizione di Boo
				float booZ = -camera->position.z;                       // Posizione Z di Boo è opposta alla posizione della camera rispetto al mirror
                Vector3f booPosition(camera->position.x, camera->position.y, booZ);

                // Setup degli uniform comuni E aggiunta della luce respiratoria personale di Boo
                booShader->setVec3("viewPosition", camera->position.x, camera->position.y, camera->position.z);

                // Applica le luci inclusa la luce di Boo dal LightManager
                if (lightManager) {
                    lightManager->applyToShaderWithBoo(booShader, camera->position, booPosition, breathingTime);
                }

				// IBL (uguale alla scena principale)  Image Based Lighting
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxManager->getSkyboxTexture());
                booShader->setInt("skybox", 1);

                // Impostazione delle matrici view e projection
                Matrix4f viewT = camera->viewMatrix;
                booShader->setMat4("view", &viewT.a11);

                Matrix4f projT = camera->projectionMatrix;
                projT.transpose();
                booShader->setMat4("projection", &projT.a11);

                // Matrice del modello Boo
                Matrix4f booModelMatrix;
                booModelMatrix.a11 = 5.0f;
                booModelMatrix.a22 = 5.0f;
                booModelMatrix.a33 = 5.0f;
                booModelMatrix.a44 = 1.0f;

                // Rotazione semplice usando direttamente gli angoli della camera
                float yawRad = (-camera->yAngle - 90.0f) * 3.14159f / 180.0f;
                float pitchRad = camera->xAngle * 3.14159f / 180.0f;

                // Rotazione Yaw (asse Y)
                Matrix4f yawMatrix;
                yawMatrix.a11 = cos(yawRad); yawMatrix.a13 = sin(yawRad);
                yawMatrix.a31 = -sin(yawRad); yawMatrix.a33 = cos(yawRad);

                // Rotazione Pitch (asse X)
                Matrix4f pitchMatrix;
                pitchMatrix.a22 = cos(pitchRad); pitchMatrix.a23 = -sin(pitchRad);
                pitchMatrix.a32 = sin(pitchRad); pitchMatrix.a33 = cos(pitchRad);

                // Matrice di scala
                Matrix4f scaleMatrix;
                scaleMatrix.a11 = 0.07f; scaleMatrix.a22 = 0.07f; scaleMatrix.a33 = 0.07f;

                // Combinazione: Scala * Yaw * Pitch
                booModelMatrix = scaleMatrix * yawMatrix * pitchMatrix;

                booModelMatrix.a41 = camera->position.x;
                booModelMatrix.a42 = camera->position.y;
                booModelMatrix.a43 = booZ;

                booModelMatrix.transpose();
                booShader->setMat4("model", &booModelMatrix.a11);

                booModel->DrawWithMaterials();
            }
        }

        // === RENDERING DEL VETRO SPORCO DELLA PORTA ===
        effectsManager->renderDirtyMirror(camera);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Pulizia delle risorse
    delete camera;
    delete lightManager;
    delete skyboxManager;
    delete modelManager;
    delete effectsManager;

    ShaderManager::cleanup();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}