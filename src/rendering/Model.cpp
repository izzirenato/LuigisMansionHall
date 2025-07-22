#include "rendering/Model.h"
#include "rendering/ShaderManager.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <assimp/stb_image.h>
#include <filesystem>
#include <fstream>
#include <sstream>

// Inizializzazione statici MaterialManager
std::map<MaterialType, MaterialProperties> MaterialManager::materials;
bool MaterialManager::initialized = false;

void MaterialManager::initialize()
{
	if (initialized) return;
	
	/*// Materiale Phong - più riflettente e metalloco
	materials[MaterialType::PHONG] = {
		Vector3f(0.2f, 0.2f, 0.2f),  // ambient
		Vector3f(0.8f, 0.8f, 0.8f),  // diffuse
		Vector3f(1.0f, 1.0f, 1.0f),  // specular
		64.0f                         // shininess
	};
	
	// Materiale Lambert - più opaco e matto
	materials[MaterialType::LAMBERT] = {
		Vector3f(0.3f, 0.3f, 0.3f),  // ambient
		Vector3f(0.7f, 0.7f, 0.7f),  // diffuse
		Vector3f(0.1f, 0.1f, 0.1f),  // specular
		8.0f                          // shininess
	};*/
	
    // Materiale Phong - più riflettente e metallico
    materials[MaterialType::PHONG] = {
        Vector3f(0.1f, 0.1f, 0.1f),  // ambient (leggero contributo ambientale)
        Vector3f(0.7f, 0.7f, 0.7f),  // diffuse (colore base)
        Vector3f(0.9f, 0.9f, 0.9f),  // specular (riflesso quasi bianco)
        50.0f                        // shininess (alto → riflessi concentrati)
    };
    materials[MaterialType::LAMBERT] = {
     Vector3f(0.1f, 0.1f, 0.1f),  // ambient
     Vector3f(0.5f, 0.5f, 0.5f),  // diffuse (ridotto da 0.7 a 0.5)
     Vector3f(0.0f, 0.0f, 0.0f),  // specular
     0.0f
    };


	initialized = true;
	std::cout << "MaterialManager initialized with Phong and Lambert materials" << std::endl;
}

MaterialProperties MaterialManager::getMaterial(MaterialType type)
{
	if (!initialized) initialize();
	return materials[type];
}

// ctor
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, std::string materialName)
    : vertices(vertices), indices(indices), textures(textures), materialName(materialName) 
{
    setupMesh();
}

void Mesh::setupMesh() 
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Posizione
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // Normale
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    // Coordinate texture
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Mesh::Draw(unsigned int shaderProgram, Model* model) 
{
    // Check if mesh has textures
    bool hasTextures = !textures.empty();
    
    // Set useTextures uniform based on whether this mesh has textures
    glUniform1i(glGetUniformLocation(shaderProgram, "useTextures"), hasTextures ? 1 : 0);
    
    // Set material properties
    if (model && !materialName.empty()) {
        MaterialProperties props = model->getMaterialProperties(materialName);
        glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), props.ambient.x, props.ambient.y, props.ambient.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), props.diffuse.x, props.diffuse.y, props.diffuse.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), props.specular.x, props.specular.y, props.specular.z);
        glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), props.shininess);
    } else {
        // Default material properties
        glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), 0.2f, 0.2f, 0.2f);
        glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), 0.8f, 0.8f, 0.8f);
        glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 32.0f);
    }
    
    if (hasTextures) {
        // Bind textures
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;

        for (unsigned int i = 0; i < textures.size(); i++) 
        {
            glActiveTexture(GL_TEXTURE0 + i);

            std::string number;
            std::string name = textures[i].type;
            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++);

            std::string uniformName = "material." + name + number;
            glUniform1i(glGetUniformLocation(shaderProgram, uniformName.c_str()), i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
    }

    // Draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Reset to default
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::DrawWithMaterial(Model* model) 
{
    if (!model || materialName.empty()) {
        std::cout << "Warning: No model or material name for mesh" << std::endl;
        return;
    }
    
    // Get material type and corresponding shader
    XMLMaterial* xmlMat = model->findXMLMaterial(materialName);
    MaterialType materialType = xmlMat ? xmlMat->materialType : MaterialType::PHONG;
    
    Shader* shader = ShaderManager::getShaderForMaterial(materialType);
    if (!shader) {
        std::cout << "Error: No shader found for material type" << std::endl;
        return;
    }
    
    // Check if mesh has textures
    bool hasTextures = !textures.empty();
    
    if (hasTextures) {
        // Bind textures - simplified since we only use diffuse
        if (!textures.empty()) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textures[0].id);
            shader->setInt("texture_diffuse1", 0);
        }
    }
    
    shader->setBool("useTextures", hasTextures);
    
    // Note: Light uniforms should be set by the renderer before calling this function
    // This includes: lightPosition, lightColor, viewPosition, ambientLight, etc.

    // Draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Reset to default
    glActiveTexture(GL_TEXTURE0);
}

// Implementazione Model
Model::Model(const std::string& path) 
{
    // Inizializza MaterialManager
    MaterialManager::initialize();
    
    // Prima carica i materiali XML
    if (std::filesystem::exists("config.xml")) {
        std::cout << "Loading XML materials..." << std::endl;
        loadXMLMaterials("config.xml");
    } else {
        std::cout << "config.xml not found!" << std::endl;
    }
    
    loadModel(path);
}

void Model::Draw(unsigned int shaderProgram) 
{
    for (unsigned int i = 0; i < meshes.size(); i++) 
    {
        meshes[i].Draw(shaderProgram, this);
    }
}

void Model::DrawWithMaterials()
{
    // Group meshes by material type to minimize shader switching
    std::map<MaterialType, std::vector<Mesh*>> meshesByType;
    
    for (unsigned int i = 0; i < meshes.size(); i++) 
    {
        XMLMaterial* xmlMat = findXMLMaterial(meshes[i].materialName);
        MaterialType materialType = xmlMat ? xmlMat->materialType : MaterialType::PHONG;
        meshesByType[materialType].push_back(&meshes[i]);
    }
    
    // Draw meshes grouped by material type
    for (auto& pair : meshesByType) {
        MaterialType materialType = pair.first;
        std::vector<Mesh*>& meshes = pair.second;
        
        Shader* shader = ShaderManager::getShaderForMaterial(materialType);
        if (!shader) continue;
        
        shader->use();
        // Note: Common uniforms (lights, camera, etc.) need to be set by the caller
        
        for (Mesh* mesh : meshes) {
            mesh->DrawWithMaterial(this);
        }
    }
}

void Model::loadModel(const std::string& path) 
{
    Assimp::Importer importer;
    
    // Combinazione dei flag originali con i nuovi flag per le normali
    const aiScene* scene = importer.ReadFile(path, 
        aiProcess_Triangulate | 
        aiProcess_FlipUVs |
        aiProcess_GenNormals |          // Genera normali se mancano
        aiProcess_GenSmoothNormals |    // Genera normali smooth
        aiProcess_FixInfacingNormals);  // Corregge normali invertite

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }

    directory = path.substr(0, path.find_last_of("/\\"));
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) 
{
    // Processa tutti i mesh del nodo
    for (unsigned int i = 0; i < node->mNumMeshes; i++) 
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    // Processa i nodi figli
    for (unsigned int i = 0; i < node->mNumChildren; i++) 
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) 
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // Processa i vertici
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) 
    {
        Vertex vertex;

        // Posizione
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;

        // Normale - ora con controllo e debug
        if (mesh->HasNormals()) 
        {
            vertex.normal.x = mesh->mNormals[i].x;
            vertex.normal.y = mesh->mNormals[i].y;
            vertex.normal.z = mesh->mNormals[i].z;
        }
        else 
        {
            // Fallback: normale verso l'alto
            vertex.normal.x = 0.0f;
            vertex.normal.y = 1.0f;
            vertex.normal.z = 0.0f;
        }

        // Coordinate texture
        if (mesh->mTextureCoords[0]) 
        {
            vertex.texCoords[0] = mesh->mTextureCoords[0][i].x;
            vertex.texCoords[1] = mesh->mTextureCoords[0][i].y;
        }
        else 
        {
            vertex.texCoords[0] = 0.0f;
            vertex.texCoords[1] = 0.0f;
        }

        vertices.push_back(vertex);
    }

    // Processa gli indici
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) 
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) 
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Processa i materiali - VERSIONE MIGLIORATA (mantieni la tua versione esistente)
    aiString materialName;
    if (mesh->mMaterialIndex >= 0) 
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        // Ottieni il nome del materiale per debug
        material->Get(AI_MATKEY_NAME, materialName);
        std::cout << "Processing material: '" << materialName.C_Str() << "'" << std::endl;

        // Debug: controlla se ci sono normali valide
        bool hasValidNormals = mesh->HasNormals();
        std::cout << "  -> Has normals: " << (hasValidNormals ? "YES" : "NO") << std::endl;

        // Prima prova a caricare texture dal materiale Assimp
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        
        std::cout << "Textures from Assimp: " << textures.size() << std::endl;
        
        // SEMPRE prova anche con XML (non solo se texture è vuoto)
        XMLMaterial* xmlMat = findXMLMaterial(std::string(materialName.C_Str()));
        if (xmlMat) {
            std::cout << "Found XML material: " << xmlMat->name << " (type: " 
                     << (xmlMat->materialType == MaterialType::LAMBERT ? "Lambert" : "Phong") << ")" << std::endl;
            
            // Carica texture se presente
            if (!xmlMat->texturePath.empty()) {
                // Controlla se la texture è già stata caricata
                bool alreadyLoaded = false;
                for (const auto& tex : textures_loaded) {
                    if (tex.path == xmlMat->texturePath) {
                        textures.push_back(tex);
                        alreadyLoaded = true;
                        break;
                    }
                }
                
                if (!alreadyLoaded) {
                    Texture texture;
                    texture.id = TextureFromFile(xmlMat->texturePath.c_str(), "");
                    texture.type = "texture_diffuse";
                    texture.path = xmlMat->texturePath;
                    textures.push_back(texture);
                    textures_loaded.push_back(texture);
                    
                    std::cout << "Loaded XML texture for material " << materialName.C_Str() 
                             << ": " << xmlMat->texturePath << std::endl;
                }
            }
            
            // Le proprietà del materiale vengono gestite dal MaterialManager
            // (non più salvate nel mesh, ma saranno usate nel rendering)
            
        } else {
            std::cout << "No XML material found for: '" << materialName.C_Str() << "'" << std::endl;
            
            // Lista tutti i materiali XML disponibili per debug
            std::cout << "Available XML materials:" << std::endl;
            for (const auto& pair : xmlMaterials) {
                std::cout << "  - '" << pair.first << "'" << std::endl;
            }
        }
        
        std::cout << "Final texture count for this mesh: " << textures.size() << std::endl;
    }

    return Mesh(vertices, indices, textures, std::string(materialName.C_Str()));
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) 
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        // Controlla se è già caricata
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++) 
        {
            if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) 
            {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }

        if (!skip) 
        {
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }

    return textures;
}

unsigned int Model::TextureFromFile(const char* path, const std::string& directory) 
{
    std::string filename = std::string(path);
    
    // Se il path è già assoluto o contiene la cartella, usalo direttamente
    if (filename.find('/') != std::string::npos || filename.find('\\') != std::string::npos) {
        // Path già specificato
    } else if (!directory.empty()) {
        // Replace models directory with textures directory
        std::string textureDir = directory;
        size_t pos = textureDir.find("models");
        if (pos != std::string::npos) {
            textureDir.replace(pos, 6, "textures");
        }
        filename = textureDir + '\\' + filename;
    }

    std::cout << "Attempting to load texture: " << filename << std::endl;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

    if (data) 
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        std::cout << "✓ Texture loaded successfully: " << filename << " (" << width << "x" << height << ")" << std::endl;
    }
    else 
    {
        std::cout << "✗ Texture failed to load at path: " << filename << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void Model::loadXMLMaterials(const std::string& xmlPath) {
    std::cout << "Opening XML file: " << xmlPath << std::endl;
    
    std::ifstream file(xmlPath);
    if (!file.is_open()) {
        std::cout << "Failed to open XML file: " << xmlPath << std::endl;
        return;
    }

    std::string line;
    std::string currentMaterial = "";
    
    while (std::getline(file, line)) {
        // Rimuovi spazi bianchi
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        // Cerca tag Material
        if (line.find("<Material name=\"") != std::string::npos) {
            size_t start = line.find("name=\"") + 6;
            size_t end = line.find("\"", start);
            if (end != std::string::npos) {
                currentMaterial = line.substr(start, end - start);
                
                XMLMaterial xmlMat;
                xmlMat.name = currentMaterial;
                
                // Cerca attributo type
                size_t typeStart = line.find("type=\"");
                if (typeStart != std::string::npos) {
                    typeStart += 6;
                    size_t typeEnd = line.find("\"", typeStart);
                    if (typeEnd != std::string::npos) {
                        std::string typeStr = line.substr(typeStart, typeEnd - typeStart);
                        if (typeStr == "lambert") {
                            xmlMat.materialType = MaterialType::LAMBERT;
                        } else {
                            xmlMat.materialType = MaterialType::PHONG; // default
                        }
                    }
                }
                
                xmlMaterials[currentMaterial] = xmlMat;
                
                std::cout << "Found XML material: '" << currentMaterial << "' type: " 
                         << (xmlMat.materialType == MaterialType::LAMBERT ? "Lambert" : "Phong") << std::endl;
            }
        }
        // Cerca tag Texture
        else if (line.find("<Texture file=\"") != std::string::npos && !currentMaterial.empty()) {
            size_t start = line.find("file=\"") + 6;
            size_t end = line.find("\"", start);
            if (end != std::string::npos) {
                std::string texturePath = line.substr(start, end - start);
                
                xmlMaterials[currentMaterial].texturePath = texturePath;
                std::cout << "  -> Texture path: " << texturePath << std::endl;
            }
        }
    }
    
    file.close();
    std::cout << "Loaded " << xmlMaterials.size() << " materials from XML" << std::endl;
}

XMLMaterial* Model::findXMLMaterial(const std::string& materialName) {
    auto it = xmlMaterials.find(materialName);
    if (it != xmlMaterials.end()) {
        return &it->second;
    }
    return nullptr;
}

MaterialProperties Model::getMaterialProperties(const std::string& materialName) {
    XMLMaterial* xmlMat = findXMLMaterial(materialName);
    if (xmlMat) {
        return MaterialManager::getMaterial(xmlMat->materialType);
    }
    
    // Default fallback
    return MaterialManager::getMaterial(MaterialType::PHONG);
}