#include "TextureManager.h"
#include "SOIL2.H"

#include <iostream>

TextureManager* TextureManager::instance = NULL;

TextureManager::TextureManager()
{

}

TextureManager::~TextureManager()
{

}

TextureManager* TextureManager::GetInstance()
{
	if (TextureManager::instance == NULL)
	{
		TextureManager::instance = new TextureManager();
	}

	return instance;
}

void TextureManager::CleanUp()
{
	std::map<std::string, Texture*>::iterator it;
	for (it = this->pathTextureMap.begin(); it != this->pathTextureMap.end(); it++)
	{
		delete it->second;
	}

	this->pathTextureMap.clear();
	this->friendlyNameTextureMap.clear();
}

Texture* TextureManager::GetTextureFromPath(std::string path)
{
	std::map<std::string, Texture*>::iterator it = this->pathTextureMap.find(path);
	if (it != this->pathTextureMap.end())
	{
		return it->second;
	}

	return NULL;
}

Texture* TextureManager::GetTextureFromFriendlyName(std::string name)
{
	std::map<std::string, Texture*>::iterator it = this->friendlyNameTextureMap.find(name);
	if (it != this->friendlyNameTextureMap.end())
	{
		return it->second;
	}

	return NULL;
}

Texture* TextureManager::LoadTexture(const std::string& path, TextureType type, const std::string& name)
{
	Texture* texture = this->GetTextureFromPath(path);
	if (texture) // This texture was already loaded from here
	{
		return texture;
	}

	texture = this->GetTextureFromFriendlyName(name);
	if (texture) // A texture already exists with this name
	{
		std::cout << "A texture already exists with friendly name '" << name << "'!" << std::endl;
		return NULL;
	}

	GLuint id;
	id = this->LoadTextureFromFile(path.c_str());
	if (id == 0)
	{
		return NULL;
	}

	std::string typeString;
	if (type == Diffuse)
	{
		typeString = "texture_diffuse";
	}
	else if (type == Specular)
	{
		typeString = "texture_specular";
	}

	texture = new Texture(id, path, typeString);
	this->pathTextureMap.insert(std::make_pair(path, texture));
	this->friendlyNameTextureMap.insert(std::make_pair(name, texture));

	return texture;
}

GLuint TextureManager::LoadTextureFromFile(const char* path)
{
	GLuint textureID = 0;

	int width, height;
	uint8_t* data = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB); // Read image file

	if (data)
	{
		// Generate a texture ID for ourselves
		glGenTextures(1, &textureID);

		glBindTexture(GL_TEXTURE_2D, textureID);

		// Filtering parameters (We use linear whichif a UV coord doesn't correspond to to a color value in the texture, it will take the average of colors from its neighbours)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Wrapping paramters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); // Tell OpenGL about our image (passes in the width, height and image data (pixels))
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else
	{
		std::cout << "Failed to load texture " << path << std::endl;
	}

	SOIL_free_image_data(data);
	return textureID;
}