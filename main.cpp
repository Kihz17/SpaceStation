#pragma once

#include "GLCommon.h"

#include <glm/glm.hpp>
#include <glm/vec3.hpp> 
#include <glm/vec4.hpp> 
#include <glm/mat4x4.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> 

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector> 
#include <fstream>
#include <sstream>
#include <algorithm>

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_opengl3.h"
#include "vendor/imgui/imgui_impl_glfw.h"

#include "ShaderManager.h"
#include "Camera.h"
#include "Light.h"
#include "ModelManager.h"
#include "TextureManager.h"
#include "LightManager.h"

const float windowWidth = 1200;
const float windowHeight = 640;
bool editMode = true;

ShaderManager gShaderManager;

Camera camera(windowHeight, windowWidth);
float moveSpeed = 1.1f;
float lightMoveSpeed = 0.01f;

int currentEditIndex = 0;

bool emergencyLightOn = false;

struct sPanel
{
	glm::vec3 currentPosition;
	glm::vec3 closedPosition;
	float openedDistance;

	bool IsOpened()
	{
		return glm::distance(currentPosition, closedPosition) >= openedDistance;
	}

	bool IsClosed()
	{
		return glm::distance(currentPosition, closedPosition) <= 0.01;
	}
};

struct sPanelLine
{
	enum ItOrder
	{
		Normal,
		Reverse
	};

	std::vector<sPanel> panels;
	glm::vec3 openDirection;
	bool opening;
	bool closing;
	ItOrder itOrder;

	void OnUpdate(float deltaTime)
	{
		if (opening) // Try to open the doors
		{
			if (itOrder == Normal)
			{
				int needsUpdateIndex = -1;
				for (int i = 0; i < panels.size(); i++)
				{
					if (panels[i].IsOpened()) // This panel has finished its opening process, move to the next one
					{
						if (i == panels.size() - 1) // All opened
						{
							emergencyLightOn = false;
							LightManager::GetInstance()->GetLight("emergency")->EditState(false);
							closing = false;
							opening = false;
						}

						continue;
					}

					needsUpdateIndex = i;
					break; // We break here because we only want one panel to move at a time
				}

				if (needsUpdateIndex == -1) // Finished opening process
				{
					return;
				}

				// We found the panel that needs updating, now iterate for start to that position an update panels
				for (int i = 0; i < std::min((int) panels.size(), needsUpdateIndex + 1); i++)
				{
					panels[i].currentPosition += (openDirection * deltaTime);
				}
			}
			else
			{
				int needsUpdateIndex = -1;
				for (int i = panels.size() - 1; i >= 0; i--)
				{
					if (panels[i].IsOpened()) // This panel has finished its opening process, move to the next one
					{
						if (i == 0) // All opened
						{
							emergencyLightOn = false;
							LightManager::GetInstance()->GetLight("emergency")->EditState(false);
							closing = false;
							opening = false;
						}
						continue;
					}

					needsUpdateIndex = i;
					break; // We break here because we only want one panel to move at a time
				}

				if (needsUpdateIndex == -1) // Finished opening process
				{
					return;
				}
					
				// We found the panel that needs updating, now iterate for start to that position an update panels
				for (int i = panels.size() - 1; i >= std::max(0, needsUpdateIndex); i--)
				{
					panels[i].currentPosition += (openDirection * deltaTime);
				}

			}

		}
		else if(closing) // Try to close the doors
		{
			if (itOrder == Normal)
			{
				int needsUpdateIndex = -1;
				for (int i = panels.size() - 1; i >= 0; i--)
				{
					if (panels[i].IsClosed()) // This panel has finished its closing process, move to the next one
					{
						if (i == 0) // All closed
						{
							emergencyLightOn = false;
							LightManager::GetInstance()->GetLight("emergency")->EditState(false);
							closing = false;
							opening = false;
						}

						continue;
					}

					panels[i].currentPosition -= (openDirection * deltaTime);
				}
			}
			else
			{
				int needsUpdateIndex = -1;
				for (int i = 0; i < panels.size(); i++)
				{
					if (panels[i].IsClosed()) // This panel has finished its closing process, move to the next one
					{
						if (i == panels.size() - 1) // All closed
						{
							emergencyLightOn = false;
							LightManager::GetInstance()->GetLight("emergency")->EditState(false);
							closing = false;
							opening = false;
						}
						continue;
					}

					panels[i].currentPosition -= (openDirection * deltaTime);
				}
			}
		}
	}
};

std::vector<sPanelLine> panelLines(8);

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Movement controls
	if (!editMode)
	{
		if (key == GLFW_KEY_W)
		{
			camera.position += camera.direction * moveSpeed;
		}
		if (key == GLFW_KEY_A)
		{
			// Rotate our camera's direction 90 degrees to the left
			glm::vec3 left;
			constexpr float theta = glm::radians(90.0f);
			left.x = camera.direction.x * cos(theta) + camera.direction.z * sin(theta);
			left.y = 0.0f;
			left.z = -camera.direction.x * sin(theta) + camera.direction.z * cos(theta);
			camera.position += left * moveSpeed;
		}
		if (key == GLFW_KEY_S)
		{
			camera.position -= camera.direction * moveSpeed;
		}
		if (key == GLFW_KEY_D)
		{
			// Rotate our camera's direction 90 degrees to the right
			glm::vec3 right;
			constexpr float theta = glm::radians(-90.0f);
			right.x = camera.direction.x * cos(theta) + camera.direction.z * sin(theta);
			right.y = 0.0f;
			right.z = -camera.direction.x * sin(theta) + camera.direction.z * cos(theta);
			camera.position += right * moveSpeed;
		}
		if (key == GLFW_KEY_SPACE)
		{
			camera.position.y += moveSpeed;
		}
	}

	// Toggle cursor view
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		editMode = !editMode;
		int cursorOption = editMode ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED;
		glfwSetInputMode(window, GLFW_CURSOR, cursorOption);
	}

	if (key == GLFW_KEY_PAGE_UP && action == GLFW_PRESS)
	{
		LightManager::GetInstance()->GetLight("emergency")->EditState(true);
		emergencyLightOn = true;
		for (sPanelLine& line : panelLines)
		{
			line.opening = false;
			line.closing = true;
		}
	}
	else if (key == GLFW_KEY_PAGE_DOWN && action == GLFW_PRESS)
	{
		LightManager::GetInstance()->GetLight("emergency")->EditState(true);
		emergencyLightOn = true;
		for (sPanelLine& line : panelLines)
		{
			line.closing = false;
			line.opening = true;
		}
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (!editMode)
	{
		camera.MoveCamera(xpos, ypos);
	}
}

bool InitializerShaders();
void LoadModels();
void DrawTunnel(const CompiledShader& shader);
void DrawHangar(const CompiledShader& shader, const std::vector<sPanelLine>& panelLines);
void SetupLights(const CompiledShader& shader);
void DrawProps(const CompiledShader& shader);
void DrawStars(const CompiledShader& shader, const std::vector<glm::vec3>& starPositions);

template <class T>
T gGetRandBetween(T LO, T HI);

int main(void)
{
	GLFWwindow* window;

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
	{
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	// Initialize our window
	window = glfwCreateWindow(windowWidth, windowHeight, "Midterm", NULL, NULL);

	// Make sure the window initialized properly
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback); // Tell GLFW where our key callbacks are
	glfwSetCursorPosCallback(window, mouse_callback); // Tell GLFW where our mouse callback is

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress); // Give glad this process ID
	glfwSwapInterval(1);

	if (!InitializerShaders())
	{
		return -1;
	}

	LoadModels();

	CompiledShader shader = *gShaderManager.pGetShaderProgramFromFriendlyName("Shader#1");

	float fpsFrameCount = 0.f;
	float fpsTimeElapsed = 0.f;

	float previousTime = static_cast<float>(glfwGetTime());

	SetupLights(shader);

	// TODO: Fill in hole by door

	// Initlaize panelLines
	float wallZ = -17.5f;
	float wallY = 0.0f;
	for (int height = 0; height < 5; height++)
	{
		wallZ = -17.5f;

		sPanelLine panelLineLeft;
		panelLineLeft.opening = false;
		panelLineLeft.closing = false;
		panelLineLeft.openDirection = glm::vec3(0.0f, 0.0f, -1.0f);
		panelLineLeft.itOrder = sPanelLine::Reverse;

		sPanelLine panelLineRight;
		panelLineRight.opening = false;
		panelLineRight.closing = false;
		panelLineRight.openDirection = glm::vec3(0.0f, 0.0f, 1.0f);
		panelLineRight.itOrder = sPanelLine::Normal;
		for (int width = 0; width < 4; width++)
		{
			if (width < 2)
			{
				glm::vec3 pos = glm::vec3(75.0f, wallY, wallZ);
				sPanel panel;
				panel.closedPosition = pos;
				panel.currentPosition = pos;
				panel.openedDistance = 10.0f;
				panelLineLeft.panels.push_back(panel);
			} 
			else
			{
				glm::vec3 pos = glm::vec3(75.0f, wallY, wallZ);
				sPanel panel;
				panel.closedPosition = pos;
				panel.currentPosition = pos;
				panel.openedDistance = 10.0f;
				panelLineRight.panels.push_back(panel);
			}
			wallZ += 10.0f;
		}

		panelLines.push_back(panelLineLeft);
		panelLines.push_back(panelLineRight);

		wallY += 5.0f;
	}

	// Init stars
	std::vector<glm::vec3> starPositions;

	unsigned int starCount = 0;
	float maxPickDistance = 1000.0f;
	while (starCount < 10000)
	{
		glm::vec3 starLocation = glm::vec3(
			gGetRandBetween<float>(-maxPickDistance, maxPickDistance),
			gGetRandBetween<float>(-maxPickDistance, maxPickDistance),
			gGetRandBetween<float>(-maxPickDistance, maxPickDistance));

		if (glm::distance(glm::vec3(0.0f), starLocation) >= maxPickDistance * 0.8f)
		{
			starPositions.push_back(starLocation);
			//
			starCount++;
		}
	}

	camera.position = glm::vec3(-5.0f, 3.0f, 2.5f);
	camera.direction = glm::vec3(1.0f, 0.0f, 0.0f);

	float emergencyLightAngle = 0.0f;

	// Our actual render loop
	while (!glfwWindowShouldClose(window))
	{
		float currentTime = static_cast<float>(glfwGetTime());
		float deltaTime = currentTime - previousTime;
		previousTime = currentTime;

		// FPS TITLE
		{
			fpsTimeElapsed += deltaTime;
			fpsFrameCount += 1.0f;
			if (fpsTimeElapsed >= 0.03f)
			{
				std::string fps = std::to_string(fpsFrameCount / fpsTimeElapsed);
				std::string ms = std::to_string(1000.f * fpsTimeElapsed / fpsFrameCount);
				std::string newTitle = "FPS: " + fps + "   MS: " + ms;
				glfwSetWindowTitle(window, newTitle.c_str());

	
				fpsTimeElapsed = 0.f;
				fpsFrameCount = 0.f;
			}
		}

		float ratio;
		int width, height;
		glm::mat4 projection;
		glm::mat4 view;

		glfwGetFramebufferSize(window, &width, &height); // Assign width and height to our window width and height
		ratio = width / (float)height;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH);
		glEnable(GL_DEPTH_TEST); // Enables the Depth Buffer, which decides which pixels will be drawn based on their depth (AKA don't draw pixels that are behind other pixels)

		glViewport(0, 0, width, height); // Specifies the transformation of device coords to window coords 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clears the buffers

		view = camera.GetViewMatrix();
		projection = glm::perspective(0.6f, ratio, 0.1f, 1000.0f);

		shader.Bind();
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "matView"), 1, GL_FALSE, glm::value_ptr(view)); // Assign new view matrix
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "matProjection"), 1, GL_FALSE, glm::value_ptr(projection)); // Assign projection
		glUniform4f(glGetUniformLocation(shader.ID, "cameraPosition"), camera.position.x, camera.position.y, camera.position.z, 1.0f);

		// Safety, mostly for first frame
		if (deltaTime == 0.0f)
		{
			deltaTime = 0.03f;
		}

		for (sPanelLine& line : panelLines)
		{
			line.OnUpdate(deltaTime);
		}

		if (emergencyLightOn)
		{
			Light* light = LightManager::GetInstance()->GetLight("emergency");
			glm::vec3 lightPos =  light->GetPosition();
			emergencyLightAngle += glm::radians(10.0f);

			glm::vec3 newPos = glm::vec3(lightPos.x + (cos(emergencyLightAngle)) * 5.0f, lightPos.y, lightPos.z + (sin(emergencyLightAngle) * 5.0f));
			glm::vec3 newDirection = glm::normalize(newPos - lightPos);
			light->EditDirection(newDirection.x, newDirection.y, newDirection.z, 1.0f);
		}

		// QUESTION 1
		DrawTunnel(shader);

		// QUESTION 2
		DrawHangar(shader, panelLines);

		// QUESTION 3
		DrawProps(shader);

		// QUESTION 4
		DrawStars(shader, starPositions);

		// Draw lights
		for (Light* light : LightManager::GetInstance()->GetLights())
		{
			ModelManager::GetInstance()->Draw("lightFrame", shader, light->GetPosition(), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
		}

		// Show what we've drawn
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	TextureManager::GetInstance()->CleanUp();
	delete TextureManager::GetInstance();

	ModelManager::GetInstance()->CleanUp();
	delete ModelManager::GetInstance();

	glfwDestroyWindow(window); // Clean up the window

	glfwTerminate(); 
	exit(EXIT_SUCCESS);
}


bool InitializerShaders()
{
	std::stringstream ss;

	// "Normal" Shader
	Shader vertexShader;
	ss << SOLUTION_DIR << "Extern\\assets\\shaders\\vertexShader.glsl";
	vertexShader.fileName = ss.str();
	ss.str("");

	Shader fragmentShader;
	ss << SOLUTION_DIR << "Extern\\assets\\shaders\\fragmentShader.glsl";
	fragmentShader.fileName = ss.str();
	ss.str("");

	bool success = gShaderManager.createProgramFromFile("Shader#1", vertexShader, fragmentShader);
	if (success)
	{
		std::cout << "Shaders compiled OK" << std::endl;
	}
	else
	{
		std::cout << "Error making shaders: " << std::endl;
		std::cout << gShaderManager.getLastError() << std::endl;
		return 1;
	}

	return success;
}

void DrawTunnel(const CompiledShader& shader)
{
	// Section 0
	ModelManager::GetInstance()->Draw("wall3", shader, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
	ModelManager::GetInstance()->Draw("wall3", shader, glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);

	// Section 1
	ModelManager::GetInstance()->Draw("wall2", shader, glm::vec3(5.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
	ModelManager::GetInstance()->Draw("wall4", shader, glm::vec3(5.0f, 0.0f, 5.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);

	// Section 2
	ModelManager::GetInstance()->Draw("wall4", shader, glm::vec3(10.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
	ModelManager::GetInstance()->Draw("wall5", shader, glm::vec3(10.0f, 0.0f, 5.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);

	// Section 3
	ModelManager::GetInstance()->Draw("wall5", shader, glm::vec3(15.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
	ModelManager::GetInstance()->Draw("wall5", shader, glm::vec3(15.0f, 0.0f, 5.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);

	// Tunnel floor
	float floorX = 0.0f;
	float lightX = -2.5f;
	for (int i = 0; i < 4; i++)
	{
		ModelManager::GetInstance()->Draw("clight", shader, glm::vec3(lightX, 5.0f, 2.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
		ModelManager::GetInstance()->Draw("floor", shader, glm::vec3(floorX, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
		ModelManager::GetInstance()->Draw("floor", shader, glm::vec3(floorX, 5.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);

		lightX += 5.0f;
		floorX += 5.0f;
	}

	// Tunnel Door
	ModelManager::GetInstance()->Draw("tdoor1", shader, glm::vec3(17.5f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
	ModelManager::GetInstance()->Draw("door", shader, glm::vec3(17.5f, 0.0f, 1.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);

}

void DrawHangar(const CompiledShader& shader, const std::vector<sPanelLine>& panelLines)
{
	float floorX = 20.0f;
	float floorZ = -(3.5f * 5.0f);

	// Draw hangar floor & ceiling
	for (int i = 0; i < 12; i++)
	{
		floorZ = -(3.5f * 5.0f); // Reset z to base value
		for (int j = 0; j < 8; j++)
		{
			ModelManager::GetInstance()->Draw("hangarFloor", shader, glm::vec3(floorX, 0.0f, floorZ), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
			ModelManager::GetInstance()->Draw("hangarFloor", shader, glm::vec3(floorX, 25.0f, floorZ), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);

			floorZ += 5.0f;
		}

		floorX += 5.0f;
	}

	// Draw hangar lights
	float lightX = 25.0f;
	for (int i = 0; i < 3; i++)
	{
		ModelManager::GetInstance()->Draw("hangarLight", shader, glm::vec3(lightX, 23.5f, -7.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
		ModelManager::GetInstance()->Draw("hangarLight", shader, glm::vec3(lightX, 23.5f, 7.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
		lightX += 20.0f;
	}


	// Left Hangar Wall
	float wallX = 15.0f;
	float wallY = 0.0f;
	for (int width = 0; width < 6; width++)
	{
		wallY = 0.0f;
		for (int height = 0; height < 5; height++)
		{
			ModelManager::GetInstance()->Draw("cwall", shader, glm::vec3(wallX, wallY, -17.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);

			wallY += 5.0f;
		}
		wallX += 10.0f;
	}

	// Right Hangar Wall
	wallX = 25.0f;
	wallY = 0.0f;
	for (int width = 0; width < 6; width++)
	{
		wallY = 0.0f;
		for (int height = 0; height < 5; height++)
		{
			ModelManager::GetInstance()->Draw("cwall", shader, glm::vec3(wallX, wallY, 22.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
			wallY += 5.0f;
		}
		wallX += 10.0f;
	}

	// Back wall, but since we can open these, they are a bit special
	for (const sPanelLine& panelLine : panelLines)
	{
		for (const sPanel& panel : panelLine.panels)
		{
			ModelManager::GetInstance()->Draw("cwall", shader, panel.currentPosition, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
		}
	}

	// Front wall (by door)
	wallX = -7.5f;
	wallY = 0.0f;
	for (int width = 0; width < 4; width++)
	{
		wallY = 0.0f;
		for (int height = 0; height < 5; height++)
		{
			if ((width == 1 || width == 2) && height == 0) // Don't draw panels blocking door
			{
				wallY += 5.0f;
				continue;
			}

			ModelManager::GetInstance()->Draw("cwall", shader, glm::vec3(15.0f, wallY, wallX), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
			wallY += 5.0f;
		}
		wallX += 10.0f;
	}

	ModelManager::GetInstance()->Draw("connector", shader, glm::vec3(15.25f, 2.5f, -3.75f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
	ModelManager::GetInstance()->Draw("connector", shader, glm::vec3(15.25f, 2.5f, 8.75f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);

	ModelManager::GetInstance()->Draw("corner", shader, glm::vec3(14.6f, 6.4f, 8.25f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
	ModelManager::GetInstance()->Draw("corner2", shader, glm::vec3(14.6f, 2.4f, 8.1f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
	ModelManager::GetInstance()->Draw("corner3", shader, glm::vec3(14.6f, 2.4f, 4.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
	ModelManager::GetInstance()->Draw("corner4", shader, glm::vec3(14.6f, 6.4f, 3.75f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
}

void SetupLights(const CompiledShader& shader)
{
	LightManager::GetInstance()->AddLight(shader, "tunnel", glm::vec3(0.0f, 2.5f, 2.5f));
	Light* tunnelLight = LightManager::GetInstance()->GetLight("tunnel");
	tunnelLight->EditLightType(Light::LightType::POINT, 0.0f, 0.0f);
	tunnelLight->EditDirection(1.0f, 0.0f, 0.0f, 1.0f);
	tunnelLight->EditDiffuse(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
	tunnelLight->EditAttenuation(0.24f, 1.35f, 0.72f, 50.0f);

	LightManager::GetInstance()->AddLight(shader, "emergency", glm::vec3(55.0f, 20.0f, 0.0f));
	Light* emergencyLight = LightManager::GetInstance()->GetLight("emergency");
	emergencyLight->EditLightType(Light::LightType::SPOT, 30.0f, 35.0f);
	emergencyLight->EditDirection(1.0f, 0.0f, 0.0f, 1.0f);
	emergencyLight->EditDiffuse(1.0f, 0.0f, 0.0f, 1.0f);
	emergencyLight->EditSpecular(1.0f, 0.0f, 0.0f, 100.0f);
	emergencyLight->EditState(false);

	LightManager::GetInstance()->AddLight(shader, "hangar", glm::vec3(50.0f, 12.0f, 0.0f));
	Light* hangarLight = LightManager::GetInstance()->GetLight("hangar");
	hangarLight->EditLightType(Light::LightType::POINT, 0.0f, 0.0f);
	hangarLight->EditDirection(0.0f, 1.0f, 0.0f, 1.0f);
	hangarLight->EditAttenuation(0.8f, 0.3f, 0.05f, 50.0f);

	float lightX = 25.0f;
	for (int i = 0; i < 3; i++)
	{
		std::stringstream ss;
		ss << "spot" << i + "_1";
		std::string firstSpot = ss.str();
		ss.str("");
		LightManager::GetInstance()->AddLight(shader, firstSpot, glm::vec3(lightX, 21.5f, -7.5f));
		Light* spot1 = LightManager::GetInstance()->GetLight(firstSpot);
		spot1->EditLightType(Light::LightType::SPOT, 2.0f, 40.0f);
		spot1->EditDirection(0.0f, -1.0f, 0.0f, 1.0f);

		ss << "spot" << i << "_2";
		std::string secondSpot = ss.str();
		ss.str("");
		LightManager::GetInstance()->AddLight(shader, secondSpot, glm::vec3(lightX, 21.5f, 7.5f));
		Light* spot2 = LightManager::GetInstance()->GetLight(secondSpot);
		if (i == 2)
		{ // Make one of the lights super bright
			spot2->EditLightType(Light::LightType::SPOT, 15.0f, 25.0f);
		}
		else
		{
			spot2->EditLightType(Light::LightType::SPOT, 2.0f, 40.0f);
		}
		
		spot2->EditDirection(0.0f, -1.0f, 0.0f, 1.0f);


		lightX += 20.0f;
	}
}

void DrawProps(const CompiledShader& shader)
{
	// Desks
	ModelManager::GetInstance()->Draw("desk1", shader, glm::vec3(20.0f, 0.0f, -10.0f), glm::vec3(0.764842f, 0.0f, -0.644218f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.644218f, 0.0f, 0.764842f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
	ModelManager::GetInstance()->Draw("desk2", shader, glm::vec3(20.0f, 0.0f, 15.0f), glm::vec3(-0.856888f, 0.0f, -0.515502f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.515502f, 0.0f, -0.856888f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);

	ModelManager::GetInstance()->Draw("smallDesk", shader, glm::vec3(70.0f, 0.0f, -10.0f), glm::vec3(-0.702712f, 0.0f, -0.711474f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.711474f, 0.0f, -0.702712f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
	ModelManager::GetInstance()->Draw("bigDesk", shader, glm::vec3(70.0f, 0.0f, 15.0f), glm::vec3(0.659983f, 0.0f, -0.751281f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.751281f, 0.0f, 0.659983f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);

	// Beakers
	ModelManager::GetInstance()->Draw("beaker", shader, glm::vec3(70.0f, 1.5f, 15.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0.5f);
	ModelManager::GetInstance()->Draw("beaker", shader, glm::vec3(69.5f, 1.5f, 16.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0.5f);
	ModelManager::GetInstance()->Draw("beaker", shader, glm::vec3(69.0f, 1.5f, 16.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0.5f);
	ModelManager::GetInstance()->Draw("beaker", shader, glm::vec3(71.5f, 1.5f, 14.2f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0.5f);

	// WhatModelsShouldIUseINFO6028Midterm.exe models
	ModelManager::GetInstance()->Draw("locker1", shader, glm::vec3(30.0f, 0.0f, -16.8f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
	ModelManager::GetInstance()->Draw("locker1", shader, glm::vec3(31.0f, 0.0f, -16.8f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
	ModelManager::GetInstance()->Draw("locker2", shader, glm::vec3(32.8f, 0.0f, -16.8f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
	ModelManager::GetInstance()->Draw("plant1", shader, glm::vec3(54.0f, 0.0f, 22.2f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
	ModelManager::GetInstance()->Draw("plant2", shader, glm::vec3(60.0f, 0.0f, 20.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
	ModelManager::GetInstance()->Draw("rocket", shader, glm::vec3(70.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
	ModelManager::GetInstance()->Draw("scales", shader, glm::vec3(70.0f, 1.5f, -10.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
	ModelManager::GetInstance()->Draw("server", shader, glm::vec3(72.5f, 0.0f, -16.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
	ModelManager::GetInstance()->Draw("sign", shader, glm::vec3(63.0f, 0.0f, 19.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
	ModelManager::GetInstance()->Draw("monitor", shader, glm::vec3(20.0f, 1.5f, 15.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
}

template <class T>
T gGetRandBetween(T LO, T HI)
{
	float fLO = static_cast<float>(LO);
	float fHI = static_cast<float>(HI);
	float r3 = fLO + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (fHI - fLO)));
	return r3;
}

void DrawStars(const CompiledShader& shader, const std::vector<glm::vec3>& starPositions)
{
	for (const glm::vec3& pos : starPositions)
	{
		ModelManager::GetInstance()->Draw("star", shader, pos, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
	}
}

void LoadModels()
{
	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\models\\ISO_Sphere.ply";
		Model* model = ModelManager::GetInstance()->LoadModel(ss.str(), "lightFrame");
		model->SetWireframe(true);
		model->SetIgnoreLighting(true);
		model->SetIsOverrideColor(true);
		model->SetColorOverride(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	}
	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\models\\ISO_Sphere.ply";
		Model* model = ModelManager::GetInstance()->LoadModel(ss.str(), "star");
		model->SetIgnoreLighting(true);
		model->SetIsOverrideColor(true);
		model->SetColorOverride(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	}
	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\models\\SM_Env_Wall_Curved_01_xyz_n_rgba_uv.ply";
		Model* model = ModelManager::GetInstance()->LoadModel(ss.str(), "wall1");
	}
	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\models\\SM_Env_Wall_Curved_02_xyz_n_rgba_uv.ply";
		Model* model = ModelManager::GetInstance()->LoadModel(ss.str(), "wall2");
	}
	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\models\\SM_Env_Wall_Curved_03_xyz_n_rgba_uv.ply";
		Model* model = ModelManager::GetInstance()->LoadModel(ss.str(), "wall3");
	}
	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\models\\SM_Env_Wall_Curved_04_xyz_n_rgba_uv.ply";
		Model* model = ModelManager::GetInstance()->LoadModel(ss.str(), "wall4");
	}
	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\models\\SM_Env_Wall_Curved_05_xyz_n_rgba_uv.ply";
		Model* model = ModelManager::GetInstance()->LoadModel(ss.str(), "wall5");
	}

	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\models\\SM_Env_Transition_Door_Curved_01_xyz_n_rgba_uv.ply";
		Model* model = ModelManager::GetInstance()->LoadModel(ss.str(), "tdoor1");
	}
	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\models\\SM_Env_Floor_04_xyz_n_rgba_uv.ply";
		Model* model = ModelManager::GetInstance()->LoadModel(ss.str(), "floor");
	}
	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\models\\SM_Env_Ceiling_Light_02_xyz_n_rgba_uv.ply";
		Model* model = ModelManager::GetInstance()->LoadModel(ss.str(), "clight");
	}
	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\models\\SM_Env_Door_01_xyz_n_rgba_uv.ply";
		Model* model = ModelManager::GetInstance()->LoadModel(ss.str(), "door");
	}
	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\models\\SM_Env_Floor_01_xyz_n_rgba_uv.ply";
		Model* model = ModelManager::GetInstance()->LoadModel(ss.str(), "hangarFloor");
	}
	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\models\\SM_Env_Construction_Wall_01_xyz_n_rgba_uv.ply";
		Model* model = ModelManager::GetInstance()->LoadModel(ss.str(), "cwall");
	}
	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\models\\SM_Env_Ceiling_Light_01_xyz_n_rgba_uv.ply";
		Model* model = ModelManager::GetInstance()->LoadModel(ss.str(), "hangarLight");
	}
	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\models\\SM_Prop_Desk_01_xyz_n_rgba_uv.ply";
		Model* model = ModelManager::GetInstance()->LoadModel(ss.str(), "desk1");
	}
	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\models\\SM_Prop_Desk_02_xyz_n_rgba_uv.ply";
		Model* model = ModelManager::GetInstance()->LoadModel(ss.str(), "desk2");
	}
	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\models\\SM_Prop_Desk_04_xyz_n_rgba_uv.ply";
		Model* model = ModelManager::GetInstance()->LoadModel(ss.str(), "smallDesk");
	}
	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\models\\SM_Prop_Desk_03_xyz_n_rgba_uv.ply";
		Model* model = ModelManager::GetInstance()->LoadModel(ss.str(), "bigDesk");
	}
	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\models\\SM_Prop_Beaker_01_xyz_n_rgba_uv.ply";
		Model* model = ModelManager::GetInstance()->LoadModel(ss.str(), "beaker");
	}

	// WhatModelsShouldIUseINFO6028Midterm.exe models
	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\models\\SM_Prop_Lockers_01_xyz_n_rgba_uv.ply";
		Model* model = ModelManager::GetInstance()->LoadModel(ss.str(), "locker1");
	}
	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\models\\SM_Prop_Lockers_02_xyz_n_rgba_uv.ply";
		Model* model = ModelManager::GetInstance()->LoadModel(ss.str(), "locker2");
	}
	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\models\\SM_Prop_Monitor_03_xyz_n_rgba_uv.ply";
		Model* model = ModelManager::GetInstance()->LoadModel(ss.str(), "monitor");
	}
	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\models\\SM_Prop_Plants_01_xyz_n_rgba_uv.ply";
		Model* model = ModelManager::GetInstance()->LoadModel(ss.str(), "plant1");
	}
	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\models\\SM_Prop_Plants_03_xyz_n_rgba_uv.ply";
		Model* model = ModelManager::GetInstance()->LoadModel(ss.str(), "plant2");
	}
	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\models\\SM_Prop_Rocket_01_xyz_n_rgba_uv.ply";
		Model* model = ModelManager::GetInstance()->LoadModel(ss.str(), "rocket");
	}
	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\models\\SM_Prop_Scales_01_xyz_n_rgba_uv.ply";
		Model* model = ModelManager::GetInstance()->LoadModel(ss.str(), "scales");
	}
	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\models\\SM_Prop_Server_01_xyz_n_rgba_uv.ply";
		Model* model = ModelManager::GetInstance()->LoadModel(ss.str(), "server");
	}
	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\models\\SM_Prop_Sign_01_xyz_n_rgba_uv.ply";
		Model* model = ModelManager::GetInstance()->LoadModel(ss.str(), "sign");
	}
	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\models\\connector.ply";
		Model* model = ModelManager::GetInstance()->LoadModel(ss.str(), "connector");
	}
	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\models\\corner.ply";
		Model* model = ModelManager::GetInstance()->LoadModel(ss.str(), "corner");
	}
	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\models\\corner2.ply";
		Model* model = ModelManager::GetInstance()->LoadModel(ss.str(), "corner2");
	}
	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\models\\corner3.ply";
		Model* model = ModelManager::GetInstance()->LoadModel(ss.str(), "corner3");
	}
	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\models\\corner4.ply";
		Model* model = ModelManager::GetInstance()->LoadModel(ss.str(), "corner4");
	}
}