#include "Camera.h"

Camera::Camera(float windowWidth, float windowHeight)
{
	this->position = glm::vec3(0.0f, 0.0f, 0.0f);
	this->up = glm::vec3(0.0f, 1.0f, 0.0f);

	this->yaw = -90.0f; // Looking down z-axis
	this->pitch = 0.0f; // Camera level with the ground
	UpdateDirection();

	// Set the initial mouse position to the center of the window
	this->lastWindowX = windowWidth / 2.0f; 
	this->lastWindowY = windowHeight / 2.0f;

	this->sensitivity = 0.1f;
}

Camera::~Camera()
{

}

void Camera::MoveCamera(float mouseX, float mouseY)
{
	// Calcualte the mouse movement between now and the last frame
	float x = (mouseX - this->lastWindowX) * this->sensitivity;
	float y = (this->lastWindowY - mouseY) * this->sensitivity; // Must be reversed because y coords are bottom to top
	this->lastWindowX = mouseX;
	this->lastWindowY = mouseY;
	this->yaw += x;
	this->pitch = std::max(-89.0f, std::min(89.0f, pitch + y)); // Cap vertical look so we can't move our camera vertically more than 180 degrees

	UpdateDirection();
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(position, position + direction, up);
}

void Camera::UpdateDirection()
{
	float xz = cos(glm::radians(pitch)); // The pitch still influences the x & z, so we must scale by this.
	this->direction.x = cos(glm::radians(yaw)) * xz;
	this->direction.y = sin(glm::radians(pitch));
	this->direction.z = sin(glm::radians(yaw)) * xz;
	glm::normalize(this->direction);
}