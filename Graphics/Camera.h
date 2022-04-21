#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <algorithm>  

class Camera
{
public:
	glm::vec3 position;
	glm::vec3 direction;

	glm::vec3 up; // An vector that represents the up direction

	Camera(float windowWidth, float windowHeight);
	~Camera();

	float yaw; // Represents our horizontal camera rotation (Turning head)
	float pitch; // Represents our vertical camera rotation (Looking up and down)

	float lastWindowX; // Keeps track of our last x pos in the GLFW window so we can calculate the offset since the last frame
	float lastWindowY; // Keeps track of our last y pos in the GLFW window so we can calculate the offset since the last frame

	float sensitivity;

	// Move the camera around according to the mouse position in the window
	void MoveCamera(float mouseX, float mouseY);

	// Gets the Camera's view matrix according to yaw/pitch using glm::lookAt
	glm::mat4 GetViewMatrix();

private:
	// Update's the camera's direction vector according to the yaw and pitch
	void UpdateDirection();
};