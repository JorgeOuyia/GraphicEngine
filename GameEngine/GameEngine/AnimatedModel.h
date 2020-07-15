#ifndef ANIMATEDMODEL_H

#define ANIMATEDMODEL_H

#define BONES_PER_VERTEX 4

#include "Model.h"

// Each vertex have 4 bones associated with his respective weights
struct VertexBoneData
{
	GLuint ids[BONES_PER_VERTEX];
	GLfloat weights[BONES_PER_VERTEX];

	VertexBoneData()
	{
		for (int i = 0; i < BONES_PER_VERTEX; i++)
		{
			ids[i] = 0u;
			weights[i] = 0.0f;
		}
	}

	void addData(const GLuint& id, const GLfloat& weight)
	{
		for (int i = 0; i < BONES_PER_VERTEX; i++)
		{
			if (weights[i] == 0.0f)
			{
				ids[i] = id;
				weights[i] = weight;
				return;
			}
		}
	}
};

struct BoneMatrix
{
	glm::mat4 offset;
	glm::mat4 finalWorldTransformation;

	BoneMatrix()
	{
		offset = glm::mat4(1.0f);
		finalWorldTransformation = glm::mat4(1.0f);
	}

	BoneMatrix(const glm::mat4& offset, const glm::mat4& finalWorldTransformation)
	{
		this->offset = offset;
		this->finalWorldTransformation = finalWorldTransformation;
	}
};

class AnimatedModel : public Model
{
public:
	AnimatedModel(std::string modelLoc, std::string vertexLoc, std::string fragmentLoc,
		glm::vec3 position, glm::vec3 scale, glm::vec3 rotation,
		bool isMainCharacter = false);

	// VIRTUAL FUNCTIONS
	virtual void update(int* keys, const GLfloat& deltaTime) {}
	virtual void render();

	// GETTERS AND SETTERS
	inline std::unordered_map<std::string, GLuint> getBoneMapping() const { return boneMapping; }
	inline void setBoneMapping(const std::unordered_map<std::string, GLuint>& boneMapping) { this->boneMapping = boneMapping; }

	inline std::vector<BoneMatrix> getBoneMatrices() const { return boneMatrices; }
	inline void setBoneMatrices(const std::vector<BoneMatrix>& boneMatrices) { this->boneMatrices = boneMatrices; }

	inline GLuint getBoneCount() const { return boneCount; }
	inline void setBoneCount(const GLuint& boneCount) { this->boneCount = boneCount; }

	~AnimatedModel();
private:
	std::unordered_map<std::string, GLuint> boneMapping;
	std::vector<BoneMatrix> boneMatrices;
	GLuint boneCount;
protected:
	void uploadSkeletalUniforms(const std::vector<glm::mat4>& transforms);

	void boneTransform(const GLfloat& timeInSeconds, std::vector<glm::mat4>& transforms);
	void readNodeHeirarchy(const GLfloat& animationTime, const aiNode* pNode, const glm::mat4& parentTransform);

	const aiNodeAnim* findNodeAnim(const aiAnimation* pAnimation, const std::string& nodeName);

	glm::vec3 interpolateScale(const GLfloat& pAnimationTime, const aiNodeAnim* pNodeAnim);
	aiQuaternion interpolateRotation(const GLfloat& pAnimationTime, const aiNodeAnim* pNodeAnim);
	glm::vec3 interpolatePosition(const GLfloat& pAnimationTime, const aiNodeAnim* pNodeAnim);

	GLuint findScale(const GLfloat& pAnimationTime, const aiNodeAnim* pNodeAnim);
	GLuint findRotation(const GLfloat& pAnimationTime, const aiNodeAnim* pNodeAnim);
	GLuint findPosition(const GLfloat& pAnimationTime, const aiNodeAnim* pNodeAnim);

	void loadNode(aiNode* node);
	void loadModel(aiMesh* mesh);
};

#endif // !ANIMATEDMODEL_H