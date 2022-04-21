#pragma once

#include "VertexInformation.h"
#include "CompiledShader.h"

#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

class Texture;
class Mesh 
{
private:
	friend class ModelManager;
	friend class Model;
	std::vector<sColoredVertex> vertices;
	std::vector<sTriangle> faces;
	std::vector<Texture*> textures;

	GLuint VAO, VBO, EBO;

	glm::vec3 offset;
	glm::vec3 orientation;
	float scale;

	bool isWireframe;
	bool ignoreLighting;
	bool isOverrideColor;

	glm::vec4 colorOverride;

	Mesh(std::vector<sColoredVertex> vertices, std::vector<sTriangle> faces);

	void SetupMesh();

	// Draws this mesh to the screen
	void Draw(const CompiledShader& shader, const glm::vec3& position, const glm::vec3& xRot, const glm::vec3& yRot, const glm::vec3& zRot, const glm::vec3& scale, float transparency);
};