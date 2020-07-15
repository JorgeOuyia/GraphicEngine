#include "TPCamera.h"

TPCamera::TPCamera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat movementSpeed, GLfloat rotationSpeed,
	Model* model)
	: Camera(startPosition, startUp, startYaw, startPitch, movementSpeed, rotationSpeed),
	player(model)
{
}

void TPCamera::keyControl(int* keys, const GLfloat& deltaTime)
{
	// NO KEY CONTROL
}

void TPCamera::mouseControl(GLfloat xChange, GLfloat yChange)
{
	xChange *= getRotationSpeed();
	yChange *= getRotationSpeed();

	verticalAngle -= yChange;
	horizontalAngle += xChange;

	if (verticalAngle < -40.0f) verticalAngle = -40.0f;
	if (verticalAngle > 40.0f) verticalAngle = 40.0f;

	GLfloat horizontalDistance = distanceFromPlayer * cos(glm::radians(verticalAngle));
	GLfloat verticalDistance = distanceFromPlayer * sin(glm::radians(verticalAngle));

	GLfloat xOffset = horizontalDistance * sin(glm::radians(-horizontalAngle));
	GLfloat zOffset = horizontalDistance * cos(glm::radians(-horizontalAngle));

	setPosition(glm::vec3(player->getPosition().x + xOffset, player->getPosition().y - verticalDistance, player->getPosition().z + zOffset));

	update();
}

glm::mat4 TPCamera::calculateViewMatrix()
{
	return glm::lookAt(getPosition(), glm::vec3(player->getPosition().x, player->getPosition().y + 4.0f, player->getPosition().z), getUp());
}

TPCamera::~TPCamera()
{
	delete player;
}