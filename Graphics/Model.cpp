#include "Model.h"
#include "Mesh.h"
#include "Texture.h"
#include "VertexInformation.h"
#include "SOIL2.H"

#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h> // Post processing flags

Model::Model()
{
	this->isWireframe = false;
}

void Model::Draw(const CompiledShader& shader, const glm::vec3& position, const glm::vec3& xRot, const glm::vec3& yRot, const glm::vec3& zRot, const glm::vec3& scale, float transparency)
{
	for (unsigned int i = 0; i < this->meshes.size(); i++)
	{
		this->meshes[i].Draw(shader, position, xRot, yRot, zRot, scale, transparency);
	}
}

std::string Model::GetFullPath() const
{
	return this->directory + this->fileName;
}

void Model::SetWireframe(bool isWireframe)
{
	this->isWireframe = isWireframe;
	for (Mesh& mesh : this->meshes)
	{
		mesh.isWireframe = isWireframe;
	}
}

void Model::SetIgnoreLighting(bool ignoreLighting)
{
	this->ignoreLighting = ignoreLighting;
	for (Mesh& mesh : this->meshes)
	{
		mesh.ignoreLighting = ignoreLighting;
	}
}

void Model::SetIsOverrideColor(bool isOverride)
{
	this->isOverrideColor = isOverride;
	for (Mesh& mesh : this->meshes)
	{
		mesh.isOverrideColor = isOverride;
	}
}

void Model::SetColorOverride(glm::vec4 colorOverride)
{
	this->colorOverride = colorOverride;
	for (Mesh& mesh : this->meshes)
	{
		mesh.colorOverride = colorOverride;
	}
}