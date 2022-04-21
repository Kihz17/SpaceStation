#include "Light.h"

#include <iostream>
#include <sstream>

static unsigned int currentLightIndex = 0;
const unsigned int maxLights = 10;

Light::Light(int index) :
	index(index), 
	position(0.0f, 0.0f, 0.0f, 0.0f), 
	diffuse(1.0f, 1.0f, 1.0f, 1.0f),
	specular(1.0f, 1.0f, 1.0f, 1.0f),
	attenuation(0.0f, 0.1f, 0.01f, 100000.0f),
	direction(0.0f, -1.0f, 0.0f, 0.0f),
	innerAngle(0.0f),
	outerAngle(0.0f),
	state(true)
{
	this->lightType = POINT;

	this->positionLocation = 0;
	this->diffuseLocation = 0;
	this->specularLocation = 0;
	this->attenuationLocation = 0;
	this->directionLocation = 0;
	this->param1Location = 0;
	this->param2Location = 0;
}

Light::~Light()
{

}

void Light::EditPosition(float x, float y, float z, float w)
{
	this->position = glm::vec4(x, y, z, w);
	glUniform4f(this->positionLocation, this->position.x, this->position.y, this->position.z, this->position.w);
}

void Light::EditDiffuse(float x, float y, float z, float w)
{
	this->diffuse = glm::vec4(x, y, z, w);
	glUniform4f(this->diffuseLocation, this->diffuse.x, this->diffuse.y, this->diffuse.z, this->diffuse.w);
}

void Light::EditSpecular(float r, float g, float b, float power)
{
	this->specular = glm::vec4(r, g, b, power);
	glUniform4f(this->specularLocation, this->specular.x, this->specular.y, this->specular.z, this->specular.w);
}

void Light::EditAttenuation(float constant, float linear, float quadratic, float distanceCutOff)
{
	this->attenuation = glm::vec4(constant, linear, quadratic, distanceCutOff);
	glUniform4f(this->attenuationLocation, this->attenuation.x, this->attenuation.y, this->attenuation.z, this->attenuation.w);
}

void Light::EditDirection(float x, float y, float z, float w)
{
	this->direction = glm::vec4(x, y, z, w);
	glUniform4f(this->directionLocation, this->direction.x, this->direction.y, this->direction.z, this->direction.w);
}

void Light::EditLightType(LightType lightType, float innerAngle, float outerAngle)
{
	this->lightType = lightType;
	this->innerAngle = innerAngle;
	this->outerAngle = outerAngle;
	glUniform4f(this->param1Location, (float)this->lightType, this->innerAngle, this->outerAngle, 1.0f);
}

void Light::EditState(bool on)
{
	this->state = on;
	glUniform4f(this->param2Location, on ? (float) GL_TRUE : (float) GL_FALSE, 1.0f, 1.0f, 1.0f);
}

void Light::SendToShader()
{	
	glUniform4f(this->positionLocation, this->position.x, this->position.y, this->position.z, this->position.w);
	glUniform4f(this->diffuseLocation, this->diffuse.x, this->diffuse.y, this->diffuse.z, this->diffuse.w);
	glUniform4f(this->specularLocation, this->specular.x, this->specular.y, this->specular.z, this->specular.w);
	glUniform4f(this->attenuationLocation, this->attenuation.x, this->attenuation.y, this->attenuation.z, this->attenuation.w);
	glUniform4f(this->directionLocation, this->direction.x, this->direction.y, this->direction.z, this->direction.w);
	glUniform4f(this->param1Location, (float)this->lightType, this->innerAngle, this->outerAngle, 1.0f);

	if (this->state)
	{
		glUniform4f(this->param2Location, (float) GL_TRUE, 1.0f, 1.0f, 1.0f);
	}
	else
	{
		glUniform4f(this->param2Location, (float) GL_FALSE, 1.0f, 1.0f, 1.0f);
	}
}

void Light::SetupUniforms(const CompiledShader& shader)
{
	GLuint shaderID = shader.ID;
	{
		std::stringstream ss;
		ss << "lightArray[" << this->index << "].position";
		this->positionLocation = glGetUniformLocation(shaderID, ss.str().c_str());
	}
	{
		std::stringstream ss;
		ss << "lightArray[" << this->index << "].diffuse";
		this->diffuseLocation = glGetUniformLocation(shaderID, ss.str().c_str());
	}
	{
		std::stringstream ss;
		ss << "lightArray[" << this->index << "].specular";
		this->specularLocation = glGetUniformLocation(shaderID, ss.str().c_str());
	}
	{
		std::stringstream ss;
		ss << "lightArray[" << this->index << "].attenuation";
		this->attenuationLocation = glGetUniformLocation(shaderID, ss.str().c_str());
	}
	{
		std::stringstream ss;
		ss << "lightArray[" << this->index << "].direction";
		this->directionLocation = glGetUniformLocation(shaderID, ss.str().c_str());
	}
	{
		std::stringstream ss;
		ss << "lightArray[" << this->index << "].param1";
		this->param1Location = glGetUniformLocation(shaderID, ss.str().c_str());
	}
	{
		std::stringstream ss;
		ss << "lightArray[" << this->index << "].param2";
		this->param2Location = glGetUniformLocation(shaderID, ss.str().c_str());
	}
}