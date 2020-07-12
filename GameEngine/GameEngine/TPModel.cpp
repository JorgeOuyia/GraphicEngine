#include "TPModel.h"

TPModel::TPModel(std::string modelLoc, std::string vertexLoc, std::string fragmentLoc, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, Camera* camera)
	: AnimatedModel(modelLoc, vertexLoc, fragmentLoc, position, scale, rotation, camera)
{

}

void TPModel::update(int* keys, const GLfloat& deltaTime)
{
}

void TPModel::render() const
{
}

TPModel::~TPModel()
{
}
