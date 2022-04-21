#pragma once

#include "GLCommon.h"
#include "Texture.h"

#include <string>
#include <map>

class TextureManager
{
public:
	enum TextureType
	{
		Diffuse,
		Specular
	};

	~TextureManager();

	static TextureManager* GetInstance();

	Texture* LoadTexture(const std::string& path, TextureType type, const std::string& name);

	Texture* GetTextureFromPath(std::string path);

	Texture* GetTextureFromFriendlyName(std::string name);

	void CleanUp();

private:
	TextureManager();
	GLuint LoadTextureFromFile(const char* path);

	static TextureManager* instance;
	std::map<std::string, Texture*> pathTextureMap;
	std::map<std::string, Texture*> friendlyNameTextureMap;
};