#pragma once

#include <string>
#include <vector>
#include <map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glad/glad.h>

#include "math/Vector3f.h"

struct Vertex
{
	Vector3f position;
	Vector3f normal;
	float texCoords[2];		// [0] = U, [1] = V
};

struct Texture
{
	unsigned int id;
	std::string type;		// "diffuse", "specular", etc.
	std::string path;		// Path to the texture file
};

// Tipi di materiale predefiniti
enum class MaterialType
{
	PHONG,
	LAMBERT
};

// Struttura per i materiali predefiniti
struct MaterialProperties
{
	Vector3f ambient;
	Vector3f diffuse;
	Vector3f specular;
	float shininess;
};

// Classe per gestire i materiali predefiniti
class MaterialManager
{
private:
	static std::map<MaterialType, MaterialProperties> materials;
	static bool initialized;
	
public:
	static void initialize();
	static MaterialProperties getMaterial(MaterialType type);
};

// Struttura per i materiali XML
struct XMLMaterial
{
	std::string name;
	std::string texturePath;
	MaterialType materialType{MaterialType::PHONG};
};

// Forward declaration
class Model;

class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	std::string materialName;  // Nome del materiale

	unsigned int VAO, VBO, EBO;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, std::string materialName = "");

	void Draw(unsigned int shaderProgram, Model* model = nullptr);
	void DrawWithMaterial(Model* model);
	void setupMesh();
};

class Model
{
public:
	std::vector<Texture> textures_loaded;
	std::vector<Mesh> meshes;
	std::string directory;
	std::map<std::string, XMLMaterial> xmlMaterials; // Aggiungi questo

	Model(const std::string& path);
	void Draw(unsigned int shaderProgram);
	void DrawWithMaterials();

	void loadModel(const std::string& path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	unsigned int TextureFromFile(const char* path, const std::string& directory);
	
	// Aggiungi questi metodi
	void loadXMLMaterials(const std::string& xmlPath);
	XMLMaterial* findXMLMaterial(const std::string& materialName);
	MaterialProperties getMaterialProperties(const std::string& materialName);
};