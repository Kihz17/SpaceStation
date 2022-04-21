#include "ShaderManager.h"
#include "Shader.h"

#include "GLCommon.h"

#include <fstream>
#include <sstream>		
#include <vector>

#include <algorithm>
#include <iterator>	
#include <iostream>

ShaderManager::ShaderManager()
{
	return;
}

ShaderManager::~ShaderManager()
{
	std::map<unsigned int, CompiledShader*>::iterator it;
	for (it = this->m_ID_to_Shader.begin(); it != this->m_ID_to_Shader.end(); it++)
	{
		delete it->second; // Free up allocated shaders
	}

	this->m_ID_to_Shader.clear();
	this->m_name_to_ID.clear();
}

bool ShaderManager::useShaderProgram(unsigned int ID)
{
	// Use the number directy... 
	// TODO: Might do a lookup to see if we really have that ID...
	glUseProgram(ID);
	return true;
}

bool ShaderManager::useShaderProgram(std::string friendlyName)
{
	std::map< std::string, unsigned int>::iterator itShad = this->m_name_to_ID.find(friendlyName);

	if (itShad == this->m_name_to_ID.end())
	{	// Didn't find it
		// Maybe set glUseProgram(0)....?
		return false;
	}

	glUseProgram(itShad->second);

	return true;
}

unsigned int ShaderManager::getIDFromFriendlyName(std::string friendlyName)
{
	std::map< std::string, unsigned int>::iterator itShad = this->m_name_to_ID.find(friendlyName);

	if (itShad == this->m_name_to_ID.end())
	{
		return 0;
	}
	return itShad->second;
}


CompiledShader* ShaderManager::pGetShaderProgramFromFriendlyName(std::string friendlyName)
{
	unsigned int shaderID = this->getIDFromFriendlyName(friendlyName);

	std::map< unsigned int, CompiledShader* >::iterator itShad = this->m_ID_to_Shader.find(shaderID);
	if (itShad == this->m_ID_to_Shader.end())
	{	
		return NULL;
	}

	return itShad->second;
}

const unsigned int MAXLINELENGTH = 65536;		// 16x1024

void ShaderManager::setBasePath(std::string basepath)
{
	this->m_basepath = basepath;
	return;
}

bool ShaderManager::m_loadSourceFromFile(Shader& shader)
{
	std::string fullFileName = this->m_basepath + shader.fileName;

	std::ifstream theFile(fullFileName.c_str());
	if (!theFile.is_open())
	{
		return false;
	}

	shader.vecSource.clear();

	char pLineTemp[MAXLINELENGTH] = { 0 };
	while (theFile.getline(pLineTemp, MAXLINELENGTH))
	{
		std::string tempString(pLineTemp);
		shader.vecSource.push_back(tempString);
	}

	theFile.close();
	return true;
}

bool ShaderManager::m_wasThereACompileError(unsigned int shaderID, std::string& errorText)
{
	errorText = "";	
	GLint isCompiled = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

		char* pLogText = new char[maxLength];
		// Fill with zeros, maybe...?
		glGetShaderInfoLog(shaderID, maxLength, &maxLength, pLogText);
		// Copy char array to string
		errorText.append(pLogText);

		// Extra code that Michael forgot wasn't there... 
		this->m_lastError.append("\n");
		this->m_lastError.append(errorText);


		delete[] pLogText;

		return true;
	}
	return false;
}

bool ShaderManager::m_wasThereALinkError(unsigned int programID, std::string& errorText)
{
	errorText = "";	

	GLint wasError = 0;
	glGetProgramiv(programID, GL_LINK_STATUS, &wasError);
	if (wasError == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &maxLength);

		char* pLogText = new char[maxLength];
		// Fill with zeros, maybe...?
		glGetProgramInfoLog(programID, maxLength, &maxLength, pLogText);
		// Copy char array to string
		errorText.append(pLogText);

		// Extra code that Michael forgot wasn't there... 
		this->m_lastError.append("\n");
		this->m_lastError.append(errorText);


		delete[] pLogText;
		return true;
	}

	return false;
}

std::string ShaderManager::getLastError(void)
{
	std::string lastErrorTemp = this->m_lastError;
	this->m_lastError = "";
	return lastErrorTemp;
}

bool ShaderManager::m_compileShaderFromSource(Shader& shader, std::string& error)
{
	error = "";
	const unsigned int MAXLINESIZE = 8 * 1024;	// About 8K PER LINE, which seems excessive
	unsigned int numberOfLines = static_cast<unsigned int>(shader.vecSource.size());

	// This is an array of pointers to strings. aka the lines of source
	char** arraySource = new char* [numberOfLines];

	// Clear array to all zeros (i.e. '\0' or null terminator)
	memset(arraySource, 0, numberOfLines);

	for (unsigned int indexLine = 0; indexLine != numberOfLines; indexLine++)
	{
		unsigned int numCharacters = (unsigned int)shader.vecSource[indexLine].length();
		// Create an array of chars for each line
		arraySource[indexLine] = new char[numCharacters + 2];		// For the '\n' and '\0' at end
		memset(arraySource[indexLine], 0, numCharacters + 2);

		// Copy line of source into array
		for (unsigned int indexChar = 0; indexChar != shader.vecSource[indexLine].length(); indexChar++)
		{
			arraySource[indexLine][indexChar] = shader.vecSource[indexLine][indexChar];
		}//for ( unsigned int indexChar = 0...

		// At a '\0' at end (just in case)
		arraySource[indexLine][numCharacters + 0] = '\n';
		arraySource[indexLine][numCharacters + 1] = '\0';
	}

//******************************************************************************************
//	// Print it out (for debug)
//	std::cout << "Source (from char** array):" << std::endl;
//	for ( unsigned int indexLine = 0; indexLine != numberOfLines; indexLine++ )
//	{
//		std::cout << "(" << indexLine << "):\t" << arraySource[indexLine] << std::endl;
//	}
//	std::cout << "END" << std::endl;
//******************************************************************************************

	glShaderSource(shader.ID, numberOfLines, arraySource, NULL);
	glCompileShader(shader.ID);

	// Get rid of the temp source "c" style array
	for (unsigned int indexLine = 0; indexLine != numberOfLines; indexLine++)
	{	// Delete this line
		delete[] arraySource[indexLine];
	}

	// And delete the original char** array
	delete[] arraySource;

	// Did it work? 
	std::string errorText = "";
	if (this->m_wasThereACompileError(shader.ID, errorText))
	{
		std::stringstream ssError;
		ssError << shader.fileName << ": ";
		ssError << shader.getShaderTypeString();
		ssError << " compile error: ";
		ssError << errorText;
		error = ssError.str();
		return false;
	}

	return true;
}

bool ShaderManager::createProgramFromFile(std::string friendlyName, Shader& vertexShad, Shader& fragShader)
{
	std::string errorText = "";

	// Shader loading happening before vertex buffer array
	vertexShad.ID = glCreateShader(GL_VERTEX_SHADER);
	vertexShad.shaderType = Shader::VERTEX_SHADER;

	// Load some text from a file...
	if (!this->m_loadSourceFromFile(vertexShad))
	{
		return false;
	}

	errorText = "";
	if (!this->m_compileShaderFromSource(vertexShad, errorText))
	{
		this->m_lastError = errorText;
		return false;
	}


	fragShader.ID = glCreateShader(GL_FRAGMENT_SHADER); // Generate OpenGL Shader ID
	fragShader.shaderType = Shader::FRAGMENT_SHADER;

	if (!this->m_loadSourceFromFile(fragShader))
	{
		return false;
	}

	if (!this->m_compileShaderFromSource(fragShader, errorText))
	{
		this->m_lastError = errorText;
		return false;
	}


	CompiledShader* curProgram = new CompiledShader();
	curProgram->ID = glCreateProgram(); // Create shader program

	glAttachShader(curProgram->ID, vertexShad.ID);
	glAttachShader(curProgram->ID, fragShader.ID);
	glLinkProgram(curProgram->ID);

	// Was there a link error? 
	errorText = "";
	if (this->m_wasThereALinkError(curProgram->ID, errorText))
	{
		std::stringstream ssError;
		ssError << "Shader program link error: ";
		ssError << errorText;
		this->m_lastError = ssError.str();
		return false;
	}

	// At this point, shaders are compiled and linked into a program
	curProgram->friendlyName = friendlyName;

	// Add the shader to the maps
	this->m_ID_to_Shader[curProgram->ID] = curProgram;
	this->m_name_to_ID[curProgram->friendlyName] = curProgram->ID;

	return true;
}
