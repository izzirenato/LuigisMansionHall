#include "ModelManager.h"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

ModelManager::ModelManager() 
    : booModel(nullptr), wallModel(nullptr), woodModel(nullptr), metalModel(nullptr), chandelierModel(nullptr) {
}

ModelManager::~ModelManager() {
    if (booModel) delete booModel;
    if (wallModel) delete wallModel;
    if (woodModel) delete woodModel;
    if (metalModel) delete metalModel;
    if (chandelierModel) delete chandelierModel;
}

Model* ModelManager::tryLoadModel(const std::vector<std::string>& paths, const std::string& modelName) {
    std::cout << "Loading " << modelName << " model..." << std::endl;
    
    for (const auto& path : paths) {
        std::cout << "Trying model path: " << fs::absolute(path) << std::endl;
        if (fs::exists(path)) {
            std::cout << "Found " << modelName << " model at: " << path << std::endl;
            Model* model = new Model(path);
            std::cout << modelName << " model loaded successfully!" << std::endl;
            return model;
        }
    }
    
    std::cerr << "Warning: " << modelName << " model not found in any of the specified paths" << std::endl;
    return nullptr;
}

bool ModelManager::loadAllModels() {
    std::cout << "=== Loading Models ===" << std::endl;
    std::cout << "Current working directory: " << fs::current_path() << std::endl;
    
    // Load Boo model
    std::vector<std::string> booPath = {
        "assets/models/boo.obj",
        "../assets/models/boo.obj",
        "../../assets/models/boo.obj"
    };
    booModel = tryLoadModel(booPath, "Boo");
    
    // Load room models (walls)
    std::vector<std::string> wallPath = {
        "../../assets/models/muri.obj",
        "../assets/models/muri.obj",
        "assets/models/muri.obj",
        "muri.obj"
    };
    wallModel = tryLoadModel(wallPath, "Room");
    
    // Load wood model (mirror room)
    std::vector<std::string> woodPath = {
        "../../assets/models/legno.obj",
        "../assets/models/legno.obj",
        "assets/models/legno.obj",
        "legno.obj"
    };
    woodModel = tryLoadModel(woodPath, "Mirror Room");
    
    // Load metal model (porta candelabro)
    std::vector<std::string> metalPath = {
        "../../assets/models/metal.obj",
        "../assets/models/metal.obj", 
        "assets/models/metal.obj",
        "metal.obj"
    };
    metalModel = tryLoadModel(metalPath, "Porta Candelabro");
    
    // Try chandelier path as fallback for porta candelabro
    std::vector<std::string> chandelierPath = {
        "../../assets/models/candelabro.obj",
        "../assets/models/candelabro.obj",
        "assets/models/candelabro.obj",
        "candelabro.obj"
    };
    chandelierModel = tryLoadModel(chandelierPath, "Chandelier (fallback)");
    
    std::cout << "=== Model Loading Complete ===" << std::endl;
    std::cout << "- Boo model: " << (booModel ? "✓" : "✗") << std::endl;
    std::cout << "- Room model: " << (wallModel ? "✓" : "✗") << std::endl;
    std::cout << "- Mirror room model: " << (woodModel ? "✓" : "✗") << std::endl;
    std::cout << "- Porta candelabro model: " << (metalModel ? "✓" : "✗") << std::endl;
    
    return areEssentialModelsLoaded();
}

void ModelManager::renderAllModels() {
    if (wallModel) wallModel->DrawWithMaterials();
    if (woodModel) woodModel->DrawWithMaterials();
    if (metalModel) metalModel->DrawWithMaterials();
	if (chandelierModel) chandelierModel->DrawWithMaterials();
}

bool ModelManager::areEssentialModelsLoaded() const {
    // Consider room model as essential, others as optional
    return wallModel != nullptr;
}