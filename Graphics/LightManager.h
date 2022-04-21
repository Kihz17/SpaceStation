#pragma once

#include "Light.h"

#include <map>
#include <string>
#include <vector>
#include <glm/vec3.hpp>

class LightManager
{
public:
	static LightManager* GetInstance();

	void AddLight(const CompiledShader& shader, std::string friendlyName, glm::vec3 position);

	Light* GetLight(unsigned int index);

	Light* GetLight(std::string lightName);

	std::vector<Light*> GetLights();

private:
	LightManager();

	static LightManager* instance;
	static const unsigned int MAX_LIGHTS = 10; // This must match the value in the fragment shader

	unsigned int lightIndex;
	Light* lights[MAX_LIGHTS];

	std::map<std::string, Light*> friendlyNameToLights;
};