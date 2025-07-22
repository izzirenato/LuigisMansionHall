#include "EffectsManager.h"
#include "../../include/rendering/ShaderManager.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <cstdlib>

EffectsManager::EffectsManager() 
    : mirrorVAO(0), mirrorVBO(0), mirrorEBO(0), dirtyMirrorTexture(0), initialized(false) {
}

EffectsManager::~EffectsManager() {
    cleanup();
}

bool EffectsManager::initialize() {
    if (initialized) return true;
    
    // Generate dirty mirror texture
    dirtyMirrorTexture = generateDirtyMirrorTexture();
    if (dirtyMirrorTexture == 0) {
        std::cerr << "Failed to generate dirty mirror texture" << std::endl;
        return false;
    }
    
    // Setup mirror geometry
    setupMirrorGeometry();
    
    initialized = true;
    return true;
}

void EffectsManager::setupMirrorGeometry() {
    float mirrorVertices[] = {
        -4.0f, -7.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
         4.0f, -7.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
         4.0f,  7.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
        -4.0f,  7.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f
    };

    unsigned int mirrorIndices[] = {
        0, 1, 2,
        0, 2, 3
    };

    glGenVertexArrays(1, &mirrorVAO);
    glGenBuffers(1, &mirrorVBO);
    glGenBuffers(1, &mirrorEBO);

    glBindVertexArray(mirrorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mirrorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mirrorVertices), mirrorVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mirrorEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(mirrorIndices), mirrorIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

unsigned int EffectsManager::generateDirtyMirrorTexture() {
    const int texSize = 512;
    unsigned char* data = new unsigned char[texSize * texSize * 4];

    for (int y = 0; y < texSize; y++) {
        for (int x = 0; x < texSize; x++) {
            int idx = (y * texSize + x) * 4;

            float baseClean = 0.9f;

            float centerX = texSize * 0.5f;
            float centerY = texSize * 0.5f;
            float distFromCenter = sqrtf((x - centerX) * (x - centerX) + (y - centerY) * (y - centerY));
            float radiusEffect = 1.0f - (distFromCenter / (texSize * 0.7f));

            float noise = (rand() % 100) / 100.0f;
            float dirtSpots = (noise > 0.85f) ? 0.3f : 1.0f;

            float smearX = sinf(y * 0.03f) * cosf(x * 0.02f) * 0.4f + 0.6f;
            float smearY = sinf(x * 0.025f) * cosf(y * 0.035f) * 0.3f + 0.7f;

            float edgeDirt = 1.0f;
            if (x < 20 || x > texSize - 20 || y < 20 || y > texSize - 20) {
                edgeDirt = 0.5f;
            }

            float waterStains = 1.0f;
            if ((x + (int)(sinf(y * 0.1f) * 5)) % 47 < 2) {
                waterStains = 0.6f;
            }

            float finalDirt = baseClean * dirtSpots * smearX * smearY * edgeDirt * waterStains;
            finalDirt = finalDirt * radiusEffect + (1.0f - radiusEffect) * 0.6f;
            finalDirt = std::max(0.7f, std::min(1.0f, finalDirt));

            data[idx] = (unsigned char)(finalDirt * 255);
            data[idx + 1] = (unsigned char)(finalDirt * 255);
            data[idx + 2] = (unsigned char)(finalDirt * 255);
            data[idx + 3] = 255;
        }
    }

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSize, texSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    delete[] data;
    return texture;
}

void EffectsManager::renderDirtyMirror(Camera* camera) {
    if (!initialized || !camera) return;
    
    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader* mirrorShader = ShaderManager::getMirrorShader();
    if (!mirrorShader) {
        glDisable(GL_BLEND);
        return;
    }
    
    mirrorShader->use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, dirtyMirrorTexture);
    mirrorShader->setInt("dirtyTexture", 0);

    mirrorShader->setVec3("viewPos", camera->position.x, camera->position.y, camera->position.z);
    mirrorShader->setFloat("time", (float)glfwGetTime());
    mirrorShader->setFloat("ghostEffect", 1.0f);

    Matrix4f mirrorModel;
    mirrorShader->setMat4("model", &mirrorModel.a11);

    Matrix4f viewT = camera->viewMatrix;
    mirrorShader->setMat4("view", &viewT.a11);

    Matrix4f projT = camera->projectionMatrix;
    projT.transpose();
    mirrorShader->setMat4("projection", &projT.a11);

    glBindVertexArray(mirrorVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glDisable(GL_BLEND);
}

void EffectsManager::cleanup() {
    if (mirrorVAO) {
        glDeleteVertexArrays(1, &mirrorVAO);
        mirrorVAO = 0;
    }
    if (mirrorVBO) {
        glDeleteBuffers(1, &mirrorVBO);
        mirrorVBO = 0;
    }
    if (mirrorEBO) {
        glDeleteBuffers(1, &mirrorEBO);
        mirrorEBO = 0;
    }
    if (dirtyMirrorTexture) {
        glDeleteTextures(1, &dirtyMirrorTexture);
        dirtyMirrorTexture = 0;
    }
    initialized = false;
}