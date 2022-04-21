#pragma once

#include "GLCommon.h"

#include <string>

class Texture
{
public:
	GLuint GetID() const;
	
	std::string GetType() const;

private:
	friend class TextureManager;
	Texture(GLuint ID, const std::string& file, const std::string& type);
	GLuint ID;
	std::string type;
	std::string path;
};