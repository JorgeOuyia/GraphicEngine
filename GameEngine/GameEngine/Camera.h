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

	// VIRTUAL FUNCTIONS
	virtual void keyControl(int* keys, const GLfloat& deltaTime) = 0;
	virtual void mouseControl(GLfloat xChange, GLfloat yChange) = 0;
	inline virtual glm::mat4 calculateViewMatrix() { return glm::lookAt(position, position + front, up); }

	// UTIL FUNCTIONS
	inline glm::mat4 calculateProjectionMatrix() { return glm::perspective(glm::radians(45.0f), (GLfloat)bufferWidth / (GLfloat)bufferHeight, 0.1f, 100.0f); }
	inline glm::vec3 calculateDirection() { return glm::normalize(front); }

	// GETTERS AND SETTERS
	inline GLFWwindow* getWindow() const { return window; }
	inline void setWindow(GLFWwindow* window) { this->window = window; }

	inline glm::vec3 getPosition() const { return position; }
	inline void setPosition(const glm::vec3& position) { this->position = position; }

	inline glm::vec3 getFront() const { return front; }
	inline void setFront(const glm::vec3& front) { this->front = front; }

	inline glm::vec3 getUp() const { return up; }
	inline void setUp(const glm::vec3& up) { this->up = up; }

	inline glm::vec3 getRight() const { return right; }
	inline void setRight(const glm::vec3& right) { this->right = right; }

	inline glm::vec3 getWorldUp() const { return worldUp; }
	inline void setWorldUp(const glm::vec3& worldUp) { this->worldUp = worldUp; }

	inline GLfloat getYaw() const { return yaw; }
	inline void setYaw(const GLfloat& yaw) { this->yaw = yaw; }

	inline GLfloat getPitch() const { return pitch; }
	inline void setPitch(const GLfloat& pitch) { this->pitch = pitch; }

	inline GLfloat getMovementSpeed() const { return movementSpeed; }
	inline void setMovementSpeed(const GLfloat& movementSpeed) { this->movementSpeed = movementSpeed; }

	inline GLfloat getRotationSpeed() const { return rotationSpeed; }
	inline void setRotationSpeed(const GLfloat& rotationSpeed) { this->rotationSpeed = rotationSpeed; }

	inline int getBufferWidth() const { return bufferWidth; }
	inline void setBufferWidth(const int& bufferWidth) { this->bufferWidth = bufferWidth; }

	inline int getBufferHeight() const { return bufferHeight; }
	inline void setBufferHeight(const int& bufferHeight) { this->bufferHeight = bufferHeight; }

	~Camera();
private:
	GLFWwindow* window;
	glm::vec3 position, front, up, right, worldUp;
	GLfloat yaw, pitch, movementSpeed, rotationSpeed;
	int bufferWidth, bufferHeight;
protected:
	virtual void update();	
};

#endif