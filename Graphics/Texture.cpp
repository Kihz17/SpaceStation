#include "Texture.h"

Texture::Texture(GLuint ID, const std::string& file, const std::string& type)
{
	this->ID = ID;
	this->path = file;
	this->type = type;
}

GLuint Texture::GetID() const
{
	return this->ID;
}

std::string Texture::GetType() const
{
	return this->type;
}
