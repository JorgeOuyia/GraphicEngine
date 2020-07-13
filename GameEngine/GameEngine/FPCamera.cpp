#include "FPCamera.h"

FPCamera::FPCamera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat movementSpeed, GLfloat rotationSpeed, int bufferWidth, int bufferHeight)
	: Camera(startPosition, startUp, startYaw, startPitch, movementSpeed, rotationSpeed, bufferWidth, bufferHeight)
{

}

void FPCamera::keyControl(int* keys, const GLfloat& deltaTime)
{
	glm::vec3 currentPosition = getPosition();

	if (keys[GLFW_KEY_W])
	{
		currentPosition += getFront() * getMovementSpeed() * deltaTime;
	}
	if (keys[GLFW_KEY_S])
	{
		currentPosition -= getFront() * getMovementSpeed() * deltaTime;
	}
	if (keys[GLFW_KEY_D])
	{
		currentPosition += getRight() * getMovementSpeed() * deltaTime;
	}
	if (keys[GLFW_KEY_A])
	{
		currentPosition -= getRight() * getMovementSpeed() * deltaTime;
	}

	setPosition(currentPosition);
}

void FPCamera::mouseControl(GLfloat xChange, GLfloat yChange)
{
	xChange *= getRotationSpeed();
	yChange *= getRotationSpeed();

	setYaw(getYaw() + xChange);
	setPitch(getPitch() + yChange);

	if (getPitch() > 89.0f) setPitch(89.0f);
	if (getPitch() < -89.0f) setPitch(-89.0f);

	update();
}

FPCamera::~FPCamera() { }