#pragma once

#include "CompiledShader.h"

#include <string>
#include <vector>
#include <map>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

class Mesh;
class Texture;
class Model
{
public:
	// Get the path of this model
	std::string GetFullPath() const;

	void SetWireframe(bool isWireframe);
	void SetIgnoreLighting(bool ignoreLighting);
	void SetIsOverrideColor(bool isOverride);
	void SetColorOverride(glm::vec4 colorOverride);
private:
	friend class ModelManager;
	std::vector<Mesh> meshes; // Holds meshes that are part of this model
	std::string directory;
	std::string fileName;

	bool isWireframe;
	bool ignoreLighting;
	bool isOverrideColor;
	glm::vec4 colorOverride;

	Model();

	// Draw this model
	void Draw(const CompiledShader& shader, const glm::vec3& position, const glm::vec3& xRot, const glm::vec3& yRot, const glm::vec3& zRot, const glm::vec3& scale, float transparency);
};