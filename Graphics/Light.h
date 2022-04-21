#pragma once

#include "GLCommon.h"
#include "CompiledShader.h"

#include <glm/glm.hpp>
#include <glm/vec3.hpp> 
#include <glm/vec4.hpp> 

class Light
{
public:
	enum LightType
	{
		POINT = 0,
		SPOT = 1,
		DIRECTIONAL = 2,
	};

	// Modifies the position of this light
	void EditPosition(float x, float y, float z, float w);

	// Modifies the diffuse of this light
	void EditDiffuse(float x, float y, float z, float w);

	// Modifies the specular color and power of this light
	void EditSpecular(float r, float g, float b, float power);

	// Modifies the attenuation this light
	void EditAttenuation(float constant, float linear, float quadratic, float distanceCutOff);

	// Modifies the direction of this light (Does nothing if we aren't a SPOT/DIRECTIONAL light)
	void EditDirection(float x, float y, float z, float w);

	// Modifies the type of light we are
	void EditLightType(LightType lightType, float innerAngle, float outerAngle);

	// Modifies if the light is on or off
	void EditState(bool on);

	// Will copy this light's information to the shader (WARNING: Make sure you call SetupUniforms() before calling this to ensure that this object's uniform locations are set.)
	void SendToShader();

	inline glm::vec4 GetPosition() const
	{
		return position;
	}

	inline glm::vec4 GetDiffuse() const
	{
		return diffuse;
	}

	inline glm::vec4 GetSpecular() const
	{
		return specular;
	}

	inline glm::vec4 GetAttenuation() const
	{
		return attenuation;
	}

	inline glm::vec4 GetDirection() const
	{
		return direction;
	}

	inline LightType GetLightType() const
	{
		return lightType;
	}

	inline float GetInnerAngle() const
	{
		return innerAngle;
	}

	inline float GetOuterAngle() const
	{
		return outerAngle;
	}

	inline bool GetState() const
	{
		return state;
	}

private:
	friend class LightManager;
	Light(int index);
	~Light();

	const unsigned int index; // Stores the index the light is at in the fragment shader

	// Lighting members
	glm::vec4 position;
	glm::vec4 diffuse;
	glm::vec4 specular;
	glm::vec4 attenuation;
	glm::vec4 direction;
	LightType lightType;
	float innerAngle;
	float outerAngle;
	bool state;

	// Uniforms
	GLuint positionLocation;
	GLuint diffuseLocation;
	GLuint specularLocation;
	GLuint attenuationLocation;
	GLuint directionLocation;
	GLuint param1Location; // vec4(lightType, innerAngle, outerAngle, ???)
	GLuint param2Location; // vec4(isLightOn, ???, ???, ???)

	void SetupUniforms(const CompiledShader& shader);
};