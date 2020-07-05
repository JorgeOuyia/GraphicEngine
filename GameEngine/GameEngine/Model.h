#ifndef MODEL_H
#define MODEL_H

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

class Model
{
public:
	Model(std::string objectLoc, std::string vertexLoc, std::string fragmentLoc,
		glm::vec3 position, glm::vec3 scale, 
		Camera* camera);

	void render();

	~Model();
private:
	std::string objectLoc, vertexLoc, fragmentLoc;
	Shader* shader;
	glm::mat4 model;
	glm::vec3 position, scale;
	Camera* camera;

	std::vector<Mesh*> meshes;
	std::vector<Texture*> textures;
	std::vector<unsigned int> meshToTex;

	void init();
	void loadNode(aiNode* node, const aiScene* scene);
	void loadModel(aiMesh* mesh, const aiScene* scene);
	void loadTextures(const aiScene* scene);
	void uploadUniforms();
};

#endif