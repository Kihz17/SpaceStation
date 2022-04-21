#pragma once

#include <string>
#include <vector>

class Shader
{
public:
	Shader();
	~Shader();

	enum eShaderType
	{
		VERTEX_SHADER,
		FRAGMENT_SHADER,
		UNKNOWN
	};

	eShaderType shaderType;
	std::string getShaderTypeString(void);

	unsigned int ID; // OpenGL ID
	std::vector<std::string> vecSource;
	bool bSourceIsMultiLine;
	std::string fileName;
};