#pragma once

#include "Model.h"
#include "Texture.h"
#include "TextureManager.h"

#include <map>
#include <string>
#include <assimp/scene.h>
#include <assimp/material.h>

class ModelManager
{
public:
	~ModelManager();

	static ModelManager* GetInstance();

	// Loads the model from file
	Model* LoadModel(std::string path, std::string friendlyName);

	Model* GetModel(std::string friendlyName);

	void Draw(std::string friendlyName, const CompiledShader& shader, const glm::vec3& position, const glm::vec3& xRot, const glm::vec3& yRot, const glm::vec3& zRot, const glm::vec3& scale, float transparency);

	void CleanUp();

private:
	// Loads an assimp node
	void LoadAssimpNode(Model* model, aiNode* node, const aiScene* scene);

	// Loads a mesh from assimp
	Mesh LoadAssimpMesh(Model* model, aiMesh* mesh, const aiScene* scene);

	// Loads assimp material textures int our texture struct
	std::vector<Texture*> LoadAssimpMaterialTextures(aiMaterial* material, aiTextureType type, TextureManager::TextureType textureType, const std::string& rootPath);

	ModelManager();

	static ModelManager* instance;
	std::map<std::string, Model*> models;
};