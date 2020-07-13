#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 worldUp, GLfloat yaw, GLfloat pitch, GLfloat movementSpeed, GLfloat rotationSpeed, int bufferWidth, int bufferHeight)
	: position(position), worldUp(worldUp), yaw(yaw), pitch(pitch), movementSpeed(movementSpeed), rotationSpeed(rotationSpeed), bufferWidth(bufferWidth), bufferHeight(bufferHeight)
{
	front = glm::vec3(0.0f, 0.0f, -1.0f);

	update();
}

Camera::~Camera()
{
}

void Camera::update()
{
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);

	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}
