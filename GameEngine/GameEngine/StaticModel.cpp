#include "StaticModel.h"

StaticModel::StaticModel(std::string modelLoc, std::string vertexLoc, std::string fragmentLoc, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, bool isMainCharacter)
	: Model(modelLoc, vertexLoc, fragmentLoc, position, scale, rotation, isMainCharacter)
{
	loadNode(getScene()->mRootNode);
}

void StaticModel::update(int* keys, const GLfloat& deltaTime)
{
	glm::vec3 position = getPosition();
	glm::vec3 forwardVector = position - getCamera()->getPosition();
	forwardVector.y = 0;
	forwardVector = glm::normalize(forwardVector);

	if (keys[GLFW_KEY_W])
	{
		position += forwardVector * getCamera()->getMovementSpeed() * deltaTime;
	}
	if (keys[GLFW_KEY_S])
	{
		position -= forwardVector * getCamera()->getMovementSpeed() * deltaTime;
	}
	if (keys[GLFW_KEY_A])
	{
		glm::vec3 leftVector = glm::cross(getCamera()->getUp(), forwardVector);
		position += leftVector * getCamera()->getMovementSpeed() * deltaTime;
	}
	if (keys[GLFW_KEY_D])
	{
		glm::vec3 rightVector = glm::cross(forwardVector, getCamera()->getUp());
		position += rightVector * getCamera()->getMovementSpeed() * deltaTime;
	}

	setPosition(position);
	calculateModel();
}

void StaticModel::render()
{
	activateShader();
	uploadUniforms();
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

StaticModel::~StaticModel()
{
}

void StaticModel::loadNode(aiNode* node)
{
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		loadModel(getScene()->mMeshes[node->mMeshes[i]]);
	}
	for (int i = 0; i < node->mNumChildren; i++)
	{
		loadNode(node->mChildren[i]);
	}
}

void StaticModel::loadModel(aiMesh* mesh)
{
	std::vector<GLfloat> vertices;
	std::vector<unsigned int> indices;

	loadGeneralModel(mesh, vertices, indices);

	addMesh(new Mesh(&vertices[0], &indices[0], vertices.size(), indices.size()));
	addMeshToTex(mesh->mMaterialIndex);
}
