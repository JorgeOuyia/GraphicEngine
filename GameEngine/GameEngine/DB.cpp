#include "DB.h"

DB::DB(int windowWidth, int windowHeight) : windowWidth(windowWidth), windowHeight(windowHeight), mouseFirstMoved(true)
{
	initOpenGL();
}

void DB::run()
{
	Model* mainPlayer = new StaticModel("../Models/spiderman.obj", STATICVERTEXSHADER, STATICFRAGMENTSHADER, glm::vec3(0.0f, 3.0f, -10.0f), glm::vec3(4.0f, 4.0f, 4.0f), glm::vec3(0.0f, 180.0f, 0.0f));
	World* world = new World(mainPlayer);
	levels.push(world);
	GLfloat lastTime = 0.0f;
	while (!glfwWindowShouldClose(window))
	{
		GLfloat now = glfwGetTime();
		GLfloat deltaTime = now - lastTime;
		lastTime = now;

		glfwPollEvents();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		World* world = levels.front();
		world->getCamera()->setBufferWidth(bufferWidth);
		world->getCamera()->setBufferHeight(bufferHeight);
		world->cameraControl(keys, deltaTime, xChange, yChange);
		world->update(keys, deltaTime);
		world->render();

		xChange = yChange = 0.0f;

		glfwSwapBuffers(window);
	}
}

DB::~DB()
{
	while (!levels.empty()) levels.pop();
	if (window != NULL)
	{
		glfwDestroyWindow(window);
		window = NULL;
	}
	glfwTerminate();
}

void DB::initOpenGL()
{
	if (!glfwInit())
	{
		printf("Cannot initialise GLFW\n");
		glfwTerminate();
		return;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	window = glfwCreateWindow(windowWidth, windowHeight, "Game engine!", NULL, NULL);
	if (!window)
	{
		printf("Error creating window\n");
		glfwTerminate();
		return;
	}

	glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);

	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetFramebufferSizeCallback(window, reshapeCallback);

	glewExperimental = GL_TRUE;

	GLenum res;
	res = glewInit();
	if (res != GLEW_OK)
	{
		printf("Cannot initialise GLEW: %s\n", glewGetErrorString(res));
		glfwDestroyWindow(window);
		glfwTerminate();
		return;
	}

	glEnable(GL_DEPTH_TEST);

	glViewport(0, 0, bufferWidth, bufferHeight);

	glfwSetWindowUserPointer(window, this);
}

void DB::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	DB* theDB = static_cast<DB*>(glfwGetWindowUserPointer(window));
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
		return;
	}
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			theDB->keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			theDB->keys[key] = false;
		}
	}
}

void DB::reshapeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void DB::mouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	DB* theDB = static_cast<DB*>(glfwGetWindowUserPointer(window));

	if (theDB->mouseFirstMoved)
	{
		theDB->lastX = xPos;
		theDB->lastY = yPos;
		theDB->mouseFirstMoved = false;
	}

	theDB->xChange = xPos - theDB->lastX;
	theDB->yChange = theDB->lastY - yPos;

	theDB->lastX = xPos;
	theDB->lastY = yPos;
}