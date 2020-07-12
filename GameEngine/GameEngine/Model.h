#ifndef MODEL_H
#define MODEL_H

#define BONES_PER_VERTEX 4

#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cmath>

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
};

class Model
{
public:
	Model(std::string objectLoc, std::string vertexLoc, std::string fragmentLoc,
		glm::vec3 position, glm::vec3 scale, glm::vec3 rotation,
		Camera* camera);

	void render();

	~Model();
private:
	Assimp::Importer importer;
	const aiScene* scene;

	std::string objectLoc, vertexLoc, fragmentLoc;
	Shader* shader;
	glm::mat4 model;
	glm::vec3 position, scale, rotation;
	Camera* camera;

	std::vector<Mesh*> meshes;
	std::vector<Texture*> textures;
	std::vector<unsigned int> meshToTex;

	std::unordered_map<std::string, GLuint> boneMapping;
	std::vector<BoneMatrix> boneMatrices;
	glm::mat4 globalInverseTransform;
	GLuint boneCount;

	void init();

	void loadNode(aiNode* node);
	void loadModel(aiMesh* mesh);
	void loadTextures();

	void uploadUniforms();
	void uploadGeneralUniforms();
	void uploadGeneralLightUniforms();
	void uploadPointLightsUniforms();
	void uploadSpotLightUniforms();
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

	glm::mat3 aiMatrix3x3ToGlm(const aiMatrix3x3& matrix);
	glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& matrix);
	glm::vec3 aiVector3DToGlm(const aiVector3D& vector);

	aiQuaternion nlerp(const aiQuaternion& a, const aiQuaternion& b, const GLfloat& blend);
};

#endif