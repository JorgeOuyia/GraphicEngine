#ifndef TPMODEL_H

#define TPMODEL_H

#include "AnimatedModel.h"

class TPModel : public AnimatedModel
{
public:
	TPModel(std::string modelLoc, std::string vertexLoc, std::string fragmentLoc,
		glm::vec3 position, glm::vec3 scale, glm::vec3 rotation,
		GLfloat movementSpeed = 5.0f);

	// INHERITED FUNCTIONS
	void update(int* keys, const GLfloat& deltaTime);
	void render();

	// GETTERS AND SETTERS
	inline bool getIsWalking() const { return isWalking; }
	inline void setIsWalking(const bool& isWalking) { this->isWalking = isWalking; }

	~TPModel();
private:
	GLfloat movementSpeed;
	bool isWalking;
};

#endif // !TPMODEL_H