#include "Shader.h"
#include "GLCommon.h"

Shader::Shader()
{
	this->ID = 0;
	this->shaderType = Shader::UNKNOWN;
	return;
}

Shader::~Shader()
{
}

std::string Shader::getShaderTypeString(void)
{
	switch (this->shaderType)
	{
	case Shader::VERTEX_SHADER:
		return "VERTEX_SHADER";
		break;
	case Shader::FRAGMENT_SHADER:
		return "FRAGMENT_SHADER";
		break;
	case Shader::UNKNOWN:
	default:
		return "UNKNOWN_SHADER_TYPE";
		break;
	}
	// Should never reach here...
	return "UNKNOWN_SHADER_TYPE";
}