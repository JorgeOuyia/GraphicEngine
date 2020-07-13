#ifndef FPCAMERA_H

#define FPCAMERA_H

#include "Camera.h"

class FPCamera : public Camera
{
public:
	FPCamera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat movementSpeed, GLfloat rotationSpeed,
		int bufferWidth, int bufferHeight);

	// INHERITED FUNCTIONS
	void keyControl(int* keys, const GLfloat& deltaTime);
	void mouseControl(GLfloat xChange, GLfloat yChange);

	~FPCamera();
private:
};

#endif // !FPCAMERA_H