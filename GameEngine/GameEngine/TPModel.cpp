#include "TPModel.h"

TPModel::TPModel(std::string modelLoc, std::string vertexLoc, std::string fragmentLoc, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, GLfloat movementSpeed)
	: AnimatedModel(modelLoc, vertexLoc, fragmentLoc, position, scale, rotation), isWalking(false), movementSpeed(movementSpeed)
{

}

void TPModel::update(int* keys, const GLfloat& deltaTime)
{
	isWalking = false;
	glm::vec3 position = getPosition();
	glm::vec3 forwardVector = position - getCamera()->getPosition();
	forwardVector.y = 0;
	forwardVector = glm::normalize(forwardVector);

	if (keys[GLFW_KEY_W])
	{
		position += forwardVector * movementSpeed * deltaTime;
		isWalking = true;
	}
	if (keys[GLFW_KEY_S])
	{
		position -= forwardVector * movementSpeed * deltaTime;
		isWalking = true;
	}
	if (keys[GLFW_KEY_A])
	{
		glm::vec3 leftVector = glm::cross(getCamera()->getUp(), forwardVector);
		position += leftVector * movementSpeed * deltaTime;
		isWalking = true;
	}
	if (keys[GLFW_KEY_D])
	{
		glm::vec3 rightVector = glm::cross(forwardVector, getCamera()->getUp());
		position += rightVector * movementSpeed * deltaTime;
		isWalking = true;
	}

	setPosition(position);
	calculateModel();
}

void TPModel::render()
{
	std::vector<glm::mat4> transforms;
	if (getScene()->HasAnimations() && isWalking)
	{
		boneTransform(glfwGetTime(), transforms);
	}
	else
	{
		transforms.resize(MAX_BONES);
		for (int i = 0; i < MAX_BONES; i++) transforms[i] = glm::mat4(1.0f);
	}
	activateShader();
	uploadUniforms();
	uploadSkeletalUniforms(transforms);
	int i = 0;
	for (Mesh* mesh : getMeshes())
	{
		if (getTextureIndex(i) < getTextures().size() && getTexture(getTextureIndex(i))) { activateTexture(getTextureIndex(i), 0); }

		mesh->render();

		if (getTextureIndex(i) < getTextures().size() && getTexture(getTextureIndex(i))) { deactivateTexture(getTextureIndex(i)); }

		i++;
	}
	deactivateShader();
}

TPModel::~TPModel()
{
}
