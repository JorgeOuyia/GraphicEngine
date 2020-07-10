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

	void addData(GLuint id, GLfloat weight)
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

class Model
{
public:
	Model(std::string objectLoc, std::string vertexLoc, std::string fragmentLoc,
		glm::vec3 position, glm::vec3 scale, glm::vec3 rotation,
		Camera* camera);

	void render();

	~Model();
private:
	std::string objectLoc, vertexLoc, fragmentLoc;
	Shader* shader;
	glm::mat4 model;
	glm::vec3 position, scale, rotation;
	Camera* camera;

	std::vector<Mesh*> meshes;
	std::vector<Texture*> textures;

	std::vector<unsigned int> meshToTex;
	std::unordered_map<std::string, GLuint> boneMapping;
	std::vector<glm::mat4> bonesTransformations;

	glm::mat4 globalInverseTransform;

	GLuint boneCount;

	void init();

	void loadNode(aiNode* node, const aiScene* scene);
	void loadModel(aiMesh* mesh, const aiScene* scene);
	void loadTextures(const aiScene* scene);
	void boneTransform(const GLfloat& seconds, const aiScene* scene);
	void readNodeHeirarchy(const GLfloat& animationTime, const aiNode* pNode, const glm::mat4& parentTransform, const aiScene* scene);

	void uploadUniforms();
	void uploadGeneralUniforms();
	void uploadGeneralLightUniforms();
	void uploadPointLightsUniforms();
	void uploadSpotLightUniforms();

	glm::mat3 aiMatrix3x3ToGlm(const aiMatrix3x3& matrix);
	glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& matrix);
};

#endif