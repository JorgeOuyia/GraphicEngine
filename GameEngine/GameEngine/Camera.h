#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>


class Camera
{
public:
	Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat movementSpeed, GLfloat rotationSpeed,
		int bufferWidth, int bufferHeight);

	inline glm::vec3 getPosition() { return position; }
	inline glm::vec3 getDirection() { return glm::normalize(front); }

	void keyControl(int* keys, const GLfloat &deltaTime);
	void mouseControl(GLfloat xChange, GLfloat yChange);

	inline glm::mat4 getViewMatrix() { return glm::lookAt(position, position + front, up); }
	inline glm::mat4 getProjectionMatrix() { return glm::perspective(glm::radians(45.0f), (GLfloat)bufferWidth / (GLfloat)bufferHeight, 0.1f, 100.0f); }

	~Camera();
private:
	glm::vec3 position, front, up, right, worldUp;
	GLfloat yaw, pitch, movementSpeed, rotationSpeed;
	int bufferWidth, bufferHeight;

	void update();
};

#endif