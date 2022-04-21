#pragma once

#include "GLCommon.h"
#include "Shader.h"

#include <map>

// Represents a shader that as been compiled successfully
class CompiledShader
{
public:
	GLuint ID = 0;
	std::string friendlyName;

	CompiledShader();

	std::map< std::string, int> mapUniformName_to_UniformLocation;

	// Returns -1 (just like OpenGL) if NOT found
	int getUniformIDFromName(std::string name);

	// Look up the uniform location and save it.
	bool LoadUniformLocation(std::string variableName);
	
	void Bind() const;
};