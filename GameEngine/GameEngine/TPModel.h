#ifndef TPMODEL_H

#define TPMODEL_H

#include "AnimatedModel.h"

class TPModel : public AnimatedModel
{
public:
	TPModel(std::string modelLoc, std::string vertexLoc, std::string fragmentLoc,
		glm::vec3 position, glm::vec3 scale, glm::vec3 rotation,
		Camera* camera);

	void update(int* keys, const GLfloat& deltaTime);
	void render() const;

	~TPModel();
private:
	
};

#endif // !TPMODEL_H