#ifndef STATICMODEL_H

#define STATICMODEL_H

#include "Model.h"

class StaticModel : public Model
{
public:
	StaticModel(std::string modelLoc, std::string vertexLoc, std::string fragmentLoc,
		glm::vec3 position, glm::vec3 scale, glm::vec3 rotation,
		bool isMainCharacter = false);

	void update(int* keys, const GLfloat& deltaTime);
	void render();

	~StaticModel();
private:

protected:
	void loadNode(aiNode* node);
	void loadModel(aiMesh* mesh);
};

#endif // !STATICMODEL_H