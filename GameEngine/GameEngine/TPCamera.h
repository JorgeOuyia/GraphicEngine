#ifndef TPCAMERA_H

#define TPCAMERA_H

#include "Camera.h"
#include "TPModel.h"

class TPCamera : public Camera
{
public:
	TPCamera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat movementSpeed, GLfloat rotationSpeed,
		Model* model = NULL);

	// INHERITED FUNCTIONS
	void keyControl(int* keys, const GLfloat& deltaTime);
	void mouseControl(GLfloat xChange, GLfloat yChange);
	glm::mat4 calculateViewMatrix();

	// GETTERS AND SETTERS
	inline Model* getPlayer() const { return player; }
	inline void setPlayer(Model* player) { this->player = player; }

	~TPCamera();
private:
	GLfloat distanceFromPlayer = 15.0f, horizontalAngle = 0.0f, verticalAngle = 0.0f;
	Model* player;
};

#endif // !TPCAMERA_H