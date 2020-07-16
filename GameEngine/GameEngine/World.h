#ifndef WORLD_H

#define WORLD_H

#include "StaticModel.h"
#include "AnimatedModel.h"
#include "TPCamera.h"

const std::string STATICVERTEXSHADER = "../Shaders/vertex.vert";
const std::string STATICFRAGMENTSHADER = "../Shaders/fragment.frag";

const std::string ANIMATEDVERTEXSHADER = "../Shaders/animated.vert";
const std::string ANIMATEDFRAGMENTSHADER = "../Shaders/animated.frag";

class World
{
public:
	World(Model* mainPlayer);

	// VIRTUAL FUNCTIONS
	virtual void render();
	virtual void update(int* keys, const GLfloat& deltaTime);

	// UTIL FUNCTIONS
	void cameraControl(int* keys, const GLfloat& deltaTime, const GLfloat& xChange, const GLfloat& yChange);

	// GETTERS AND SETTERS
	inline std::vector<Model*> getModels() const { return models; }
	inline void setModels(const std::vector<Model*>& models) { this->models = models; }

	inline Model* getMainPlayer() const { return mainPlayer; }
	inline void setMainPlayer(Model* mainPlayer) { this->mainPlayer = mainPlayer; }

	inline Camera* getCamera() const { return camera; }
	inline void setCamera(Camera* camera) { this->camera = camera; }

	inline bool getEndWorld() const { return endWorld; }
	inline void setEndWorld(const bool& endWorld) { this->endWorld = endWorld; }

	~World();
private:
	std::vector<Model*> models;
	Model* mainPlayer;
	Camera* camera;
	bool endWorld;
protected:
	PhysicsCommon physicsManager;
	PhysicsWorld* world;

	virtual void init();

	void addStaticModel(const std::string& fileLoc, const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f), const glm::vec3& scale = glm::vec3(1.0f, 1.0f, 1.0f), const glm::vec3& rotation = glm::vec3(0.0f, 0.0f, 0.0f));
	void addAnimatedModel(const std::string& fileLoc, const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f), const glm::vec3& scale = glm::vec3(0.0f, 0.0f, 0.0f), const glm::vec3& rotation = glm::vec3(0.0f, 0.0f, 0.0f));
};

#endif // !WORLD_H