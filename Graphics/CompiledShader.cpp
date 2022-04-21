#include "CompiledShader.h"


CompiledShader::CompiledShader()
{
}

int CompiledShader::getUniformIDFromName(std::string name)
{
	std::map< std::string, int>::iterator itUniform = this->mapUniformName_to_UniformLocation.find(name);

	if (itUniform == this->mapUniformName_to_UniformLocation.end())
	{
		return -1;
	}

	return itUniform->second;
}


bool CompiledShader::LoadUniformLocation(std::string variableName)
{
	GLint uniLocation = glGetUniformLocation(this->ID, variableName.c_str());

	// Did it find it (not -1)
	if (uniLocation == -1)
	{
		return false;
	}

	// Save it
	this->mapUniformName_to_UniformLocation[variableName.c_str()] = uniLocation;
	return true;
}

void CompiledShader::Bind() const
{
	glUseProgram(this->ID);
}