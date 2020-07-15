#ifndef DB_H
#define DB_H

#include "World.h"

#include <queue>

class DB
{
public:
	DB(int windowWidth, int windowHeight);

	void run();

	~DB();
private:
	int windowWidth, windowHeight, bufferWidth, bufferHeight;
	GLFWwindow* window;
	int keys[1024];
	bool mouseFirstMoved;
	GLfloat lastX, lastY, xChange, yChange;

	std::queue<World*> levels;

	void initOpenGL();

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void reshapeCallback(GLFWwindow* window, int width, int height);
	static void mouseCallback(GLFWwindow* window, double xPos, double yPos);
};

#endif // !DB_H