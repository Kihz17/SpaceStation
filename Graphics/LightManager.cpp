#include "LightManager.h"

#include <iostream>
LightManager* LightManager::instance = NULL;

LightManager::LightManager()
{

}

LightManager* LightManager::GetInstance()
{
	if (LightManager::instance == NULL)
	{
		LightManager::instance = new LightManager();
	}

	return instance;
}

Light* LightManager::GetLight(unsigned int index)
{
	if (index > (MAX_LIGHTS - 1))
	{
		return NULL;
	}

	return this->lights[index];
}

Light* LightManager::GetLight(std::string lightName)
{
	std::map<std::string, Light*>::iterator it = this->friendlyNameToLights.find(lightName);
	if (it != this->friendlyNameToLights.end())
	{
		return it->second;
	}

	return NULL;
}

void LightManager::AddLight(const CompiledShader& shader, std::string friendlyName, glm::vec3 position)
{
	if (this->lightIndex >= this->MAX_LIGHTS)
	{
		std::cout << "The maximum number of lights has been reached. You must increase the MAX_LIGHTS value in LightManager.h AND the fragment shader to inccrease the number of lights." << std::endl;
		return;
	}

	std::map<std::string, Light*>::iterator lightIt = this->friendlyNameToLights.find(friendlyName);
	if (lightIt != this->friendlyNameToLights.end())
	{
		std::cout << "Friendly name '" << friendlyName << "' already exists in the light map." << std::endl;
		return;
	}

	unsigned int lightIndex = this->lightIndex++;
	Light* light = new Light(lightIndex);
	light->position = glm::vec4(position, 1.0f);

	shader.Bind();
	light->SetupUniforms(shader); // Make sure we setup uniform locations so that we can pass light related info to the GPU
	light->SendToShader();

	this->lights[lightIndex] = light;
	this->friendlyNameToLights.insert(std::make_pair(friendlyName, light));
}

std::vector<Light*> LightManager::GetLights()
{
	std::vector<Light*> lights;

	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		if (this->lights[i] != NULL)
		{
			lights.push_back(this->lights[i]);
		}
	}
	
	return lights;
}