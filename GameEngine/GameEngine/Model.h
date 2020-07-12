#ifndef MODEL_H
#define MODEL_H

#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "Util.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cmath>

class Model
{
public:
	Model(std::string modelLoc, std::string vertexLoc, std::string fragmentLoc,
		glm::vec3 position, glm::vec3 scale, glm::vec3 rotation,
		Camera* camera);

	// VIRTUAL FUNCTIONS
	virtual void update(int* keys, const GLfloat& deltaTime) = 0;
	virtual void render() = 0;

	// GETTERS AND SETTERS
	inline Assimp::Importer getImporter() const { return importer; }
	inline void setImporter(const Assimp::Importer& importer) { this->importer = importer; }

	inline const aiScene* getScene() const { return scene; }
	inline void setScene(const aiScene* scene) { this->scene = scene; }

	inline std::string getModelLoc() const { return modelLoc; }
	inline void setModelLoc(const std::string& modelLoc) { this->modelLoc = modelLoc; }

	inline std::string getVertexLoc() const { return vertexLoc; }
	inline void setVertexLoc(const std::string& vertexLoc) { this->vertexLoc = vertexLoc; }

	inline std::string getFragmentLoc() const { return fragmentLoc; }
	inline void setFragmentLoc(const std::string& fragmentLoc) { this->fragmentLoc = fragmentLoc; }

	inline Shader* getShader() const { return shader; }
	inline void setShader(Shader* shader) { this->shader = shader; }

	inline glm::mat4 getModel() const { return model; }
	inline void setModel(const glm::mat4& model) { this->model = model; }

	inline glm::vec3 getPosition() const { return this->position; }
	inline void setPosition(const glm::vec3& position) { this->position = position; }

	inline glm::vec3 getScale() const { return scale; }
	inline void setScale(const glm::vec3& scale) { this->scale = scale; }

	inline glm::vec3 getRotation() const { return rotation; }
	inline void setRotation(const glm::vec3& rotation) { this->rotation = rotation; }

	inline Camera* getCamera() const { return camera; }
	inline void setCamera(Camera* camera) { this->camera = camera; }

	inline std::vector<Mesh*> getMeshes() const { return meshes; }
	inline void setMeshes(const std::vector<Mesh*>& meshes) { this->meshes = meshes; }

	inline std::vector<Texture*> getTextures() const { return textures; }
	inline void setTextures(const std::vector<Texture*>& textures) { this->textures = textures; }

	inline std::vector<unsigned int> getMeshToTex() const { return meshToTex; }
	inline void setMeshToTex(const std::vector<unsigned int>& meshToTex) { this->meshToTex = meshToTex; }

	inline glm::mat4 getGlobalInverseTransform() const { return globalInverseTransform; }
	inline void setGlobalInverseTransform(const glm::mat4& globalInverseTransform) { this->globalInverseTransform = globalInverseTransform; }

	~Model();
private:
	Assimp::Importer importer;
	const aiScene* scene;

	std::string modelLoc, vertexLoc, fragmentLoc;
	Shader* shader;
	glm::mat4 model;
	glm::vec3 position, scale, rotation;
	Camera* camera;

	std::vector<Mesh*> meshes;
	std::vector<Texture*> textures;
	std::vector<unsigned int> meshToTex;

	glm::mat4 globalInverseTransform;

	void init();

	void uploadGeneralUniforms();
	void uploadGeneralLightUniforms();
	void uploadPointLightsUniforms();
	void uploadSpotLightUniforms();
protected:
	void loadTextures();
	void loadGeneralModel(aiMesh* mesh, std::vector<GLfloat>& vertices, std::vector<unsigned int>& indices);

	void calculateModel();

	void uploadUniforms();

	inline void addMesh(Mesh* mesh) { meshes.push_back(mesh); }
	inline void addMeshToTex(const unsigned int& materialIndex) { meshToTex.push_back(materialIndex); }

	inline void activateShader() const { shader->use(); }
	inline void deactivateShader() const { shader->unuse(); }

	inline unsigned int getTextureIndex(const unsigned int& index) const { return meshToTex[index]; }
	inline Texture* getTexture(const unsigned int& index) const { return textures[index]; }

	inline void activateTexture(const unsigned int& index, const unsigned int& textureUnit) const { textures[index]->use(textureUnit); }
	inline void deactivateTexture(const unsigned int& index) const { textures[index]->unuse(); }

	virtual void loadModel(aiMesh* mesh) = 0;
	virtual void loadNode(aiNode* node) = 0;
};

#endif