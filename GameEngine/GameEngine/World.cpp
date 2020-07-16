#include "World.h"

World::World(Model* mainPlayer)
	: mainPlayer(mainPlayer), endWorld(false), world(physicsManager.createPhysicsWorld()),
	camera(new TPCamera(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.5f, mainPlayer))
{
	init();
}

void World::render()
{
	for (Model* model : models)
	{
		model->render();
	}
}

void World::update(int* keys, const GLfloat& deltaTime)
{
	mainPlayer->update(keys, deltaTime);
}

void World::cameraControl(int* keys, const GLfloat& deltaTime, const GLfloat& xChange, const GLfloat& yChange)
{
	camera->keyControl(keys, deltaTime);
	camera->mouseControl(xChange, yChange);
}

World::~World()
{
}

void World::init()
{
	mainPlayer->setCamera(camera);
	models.push_back(mainPlayer);
	addStaticModel("../Models/scenario.obj", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.6f, 0.6f, 0.6f));
}

void World::addStaticModel(const std::string& fileLoc, const glm::vec3& position, const glm::vec3& scale, const glm::vec3& rotation)
{
	Model* newModel = new StaticModel(fileLoc, STATICVERTEXSHADER, STATICFRAGMENTSHADER, position, scale, rotation);
	newModel->setCamera(camera);
	models.push_back(newModel);
}

void World::addAnimatedModel(const std::string& fileLoc, const glm::vec3& position, const glm::vec3& scale, const glm::vec3& rotation)
{
	Model* newModel = new AnimatedModel(fileLoc, ANIMATEDVERTEXSHADER, ANIMATEDFRAGMENTSHADER, position, scale, rotation);
	newModel->setCamera(camera);
	models.push_back(newModel);
}