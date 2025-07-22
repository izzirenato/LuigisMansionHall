#pragma once
#include "Vector3f.h"
#include "Vector4f.h"
#include "Matrix4f.h"
#include "Quaternion.h"

class Camera
{
public:
    // Matrici di trasformazione
    Matrix4f viewMatrix;
    Matrix4f projectionMatrix;

    // Vettori della camera
    Vector3f position;
    Vector3f lookAt;
    Vector3f front;
    Vector3f up;
    Vector3f right;
    Vector3f worldUp;

    // Parametri della camera
    float angle;    // FOV
    float far;      // Far plane
    float near;     // Near plane
    float speed;    // Velocità di movimento
    float horizontalSensitivity; // Sensibilità rotazione
    float verticalSensitivity; // Sensibilità rotazione

    // Angoli di rotazione
    float xAngle;   // Pitch (inclinazione su/giù)
    float yAngle;   // Yaw (rotazione sinistra/destra)

    // Dimensioni schermo
    unsigned int screenWidth;
    unsigned int screenHeight;

    // Costruttore
    Camera(float angle, float far, float near, const Vector3f& position, const Vector3f& lookAt);

    // Metodi pubblici
    void setup(unsigned int width, unsigned int height);
    void updateCamera();
    void updateProjectionMatrix();
    void rotationY(float direction, float deltaTime);
    void rotationX(float direction, float deltaTime);
    void move(char direction, float deltaTime);
};