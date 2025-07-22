#include "math/Camera.h"
#include <cmath>
#include <cstdio>

// Helper per conversione gradi → radianti
inline float radians(float degrees)
{
    return degrees * 3.14159265359f / 180.0f;
}

Camera::Camera(float angle, float far, float near, const Vector3f& position, const Vector3f& lookAt)
{
    this->angle = angle;
    this->far = far;
    this->near = near;
    this->position = position;
    this->lookAt = lookAt;

    front = Vector3f(0.0f, 0.0f, -1.0f);
    up = Vector3f(0.0f, 1.0f, 0.0f);
    right = Vector3f(1.0f, 0.0f, 0.0f);

    speed = 5.0f;

    worldUp = Vector3f(0.0f, 1.0f, 0.0f);

    horizontalSensitivity = 0.2f;
    verticalSensitivity = 0.15f;

    xAngle = 0.0f;  // pitch
    yAngle = -90.0f;  // yaw

    updateCamera();
}

void Camera::setup(unsigned int width, unsigned int height)
{
    screenWidth = width;
    screenHeight = height;
    updateProjectionMatrix();
}

void Camera::updateCamera()
{
    // Limita il pitch (xAngle)
    if (xAngle > 89.0f) xAngle = 89.0f;
    if (xAngle < -89.0f) xAngle = -89.0f;

    float yawRad = radians(yAngle);
    float pitchRad = radians(xAngle);

    // Calcola front
    front.x = cos(pitchRad) * cos(yawRad);
    front.y = sin(pitchRad);
    front.z = cos(pitchRad) * sin(yawRad);
    front.normalize();

    // Calcola right direttamente dal yaw (sempre nel piano orizzontale)
    right.x = -sin(yawRad);
    right.y = 0.0f;
    right.z = cos(yawRad);
    right.normalize();

    // Calcola up come prodotto vettoriale
    up = right.cross(front).getNormalized();

    lookAt = position + front;
    viewMatrix.lookAt(position, lookAt, up);
}

void Camera::updateProjectionMatrix()
{
    float aspectRatio = (float)screenWidth / (float)screenHeight;
    float fovRad = radians(angle);
    float top = near * tan(fovRad / 2.0f);
    float bottom = -top;
    float rightProj = top * aspectRatio;
    float left = -rightProj;

    // Debug output - only on first setup
    static bool firstSetup = true;
    if (firstSetup) {
        printf("Screen: %dx%d, Aspect: %.3f, FOV: %.1f\n", screenWidth, screenHeight, aspectRatio, angle);
        firstSetup = false;
    }

    projectionMatrix.projection(left, rightProj, bottom, top, near, far);
}

void Camera::rotationY(float direction, float deltaTime)
{
    // Usa deltaTime per rotazione indipendente da frame rate
    yAngle += direction * horizontalSensitivity * deltaTime;
    updateCamera();
}

void Camera::rotationX(float direction, float deltaTime)
{
    // Usa deltaTime per rotazione indipendente da frame rate
    xAngle += direction * verticalSensitivity * deltaTime;
    updateCamera();
}

void Camera::move(char direction, float deltaTime)
{
    if (position.x <= -16.75)
		position.x = -16.75f; // Limite sinistro
	if (position.x >= 16.75)
		position.x = 16.75f; // Limite destro
	if (position.y <= 3.0)
		position.y = 3.0f; // Limite inferiore
	if (position.y >= 18.80f)
		position.y = 18.80f; // Limite superiore
    if (position.z<= 4.0f)
		position.z = 4.0f; // Limite anteriore
	if (position.z >= 17.0f)
		position.z = 17.0f; // Limite posteriore

    float velocity = speed * deltaTime;

    switch (direction)
    {
    case 'W': // Forward
        position += front * velocity;
        break;
    case 'S': // Backward
        position -= front * velocity;
        break;
    case 'A': // Left
        position -= right * velocity;
        break;
    case 'D': // Right
        position += right * velocity;
        break;
    case 'U': // Up
        position += worldUp * velocity;
        break;
    case 'N': // Down
        position -= worldUp * velocity;
        break;
    }

    updateCamera();
}
