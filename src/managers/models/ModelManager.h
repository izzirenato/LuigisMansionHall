#ifndef MODEL_MANAGER_H
#define MODEL_MANAGER_H

#include <vector>
#include <string>
#include <memory>
#include "../../../include/rendering/Model.h"

class ModelManager {
private:
    Model* booModel;
    Model* wallModel;
    Model* woodModel;
    Model* metalModel;
	Model* chandelierModel;
    
    // Helper to try loading model from multiple paths
    Model* tryLoadModel(const std::vector<std::string>& paths, const std::string& modelName);
    
public:
    ModelManager();
    ~ModelManager();
    
    // Load all models from predefined paths
    bool loadAllModels();
    
    // Getters
    Model* getBooModel() const { return booModel; }
    Model* getRoomModel() const { return wallModel; }
    Model* getMirrorRoomModel() const { return woodModel; }
    Model* getPortaCandelabroModel() const { return metalModel; }
	Model* getChandelierModel() const { return chandelierModel; }
    
    // Render all models
    void renderAllModels();
    
    // Check if all essential models are loaded
    bool areEssentialModelsLoaded() const;
};

#endif // MODEL_MANAGER_H