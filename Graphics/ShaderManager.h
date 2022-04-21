#ifndef _ShaderManager_HG_
#define _ShaderManager_HG_

#include "CompiledShader.h"

#include <string>
#include <vector>
#include <map>

class ShaderManager
{
public:
	ShaderManager();
	~ShaderManager();

	bool useShaderProgram(unsigned int ID);

	bool useShaderProgram(std::string friendlyName);

	bool createProgramFromFile(std::string friendlyName, Shader& vertexShad, Shader& fragShader);

	void setBasePath(std::string basepath);

	unsigned int getIDFromFriendlyName(std::string friendlyName);

	// Used to load the uniforms. Returns NULL if not found.
	CompiledShader* pGetShaderProgramFromFriendlyName(std::string friendlyName);

	// Clears last error
	std::string getLastError(void);
private:
	std::string m_basepath;
	std::string m_lastError;
	std::map< unsigned int, CompiledShader*> m_ID_to_Shader;
	std::map< std::string, unsigned int> m_name_to_ID;

	// Returns an empty string if it didn't work
	bool m_loadSourceFromFile(Shader& shader);

	bool m_compileShaderFromSource(Shader& shader, std::string& error);

	// returns false if no error
	bool m_wasThereACompileError(unsigned int shaderID, std::string& errorText);

	bool m_wasThereALinkError(unsigned int progID, std::string& errorText);
};
#endif