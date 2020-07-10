#include "Model.h"

Model::Model(std::string objectLoc, std::string vertexLoc, std::string fragmentLoc, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, Camera* camera)
	: objectLoc(objectLoc), vertexLoc(vertexLoc), fragmentLoc(fragmentLoc),
	position(position), scale(scale), rotation(rotation),
	camera(camera),
	boneCount(0)
{
	init();
}

void Model::render()
{
	shader->use();
	uploadUniforms();
	int i = 0;
	for (Mesh* mesh : meshes)
	{
		unsigned int textureIndex = meshToTex[i];

		if (textureIndex < textures.size() && textures[textureIndex])
		{
			textures[textureIndex]->use(0);
		}

		mesh->render();

		if (textureIndex < textures.size() && textures[textureIndex])
		{
			textures[textureIndex]->unuse();
		}

		i++;
	}
	shader->unuse();
}

Model::~Model()
{
	delete camera;
	delete shader;
	meshes.clear();
	textures.clear();
}

void Model::init()
{
	shader = new Shader(vertexLoc, fragmentLoc,
		DirectionalLight(glm::vec3(1.0f, 1.0f, 1.0f), 0.4f, 0.1f, glm::vec3(0.0f, 0.0f, -1.0f)),
		SpecularLight(1.0f, 250.0f),
		SpotLight(glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 0.0f, 0.0f, glm::vec3(0.0f, 0.0f, -1.0f), glm::radians(10.0f)));

	shader->addPointLight(PointLight(glm::vec3(1.0f, 0.0f, 0.0f), 0.2f, 0.2f, glm::vec3(0.0f, 0.0f, -15.0f), 0.3f, 0.2f, 0.1f));
	shader->addPointLight(PointLight(glm::vec3(0.0f, 1.0f, 0.0f), 0.2f, 0.2f, glm::vec3(10.0f, 0.0f, -15.0f), 0.3f, 0.2f, 0.1f));
	shader->addPointLight(PointLight(glm::vec3(0.0f, 0.0f, 1.0f), 0.2f, 1.2f, glm::vec3(-10.0f, 0.0f, -15.0f), 0.8f, 0.2f, 0.1f));

	model = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), scale)
		* glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1, 0, 0))
		* glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0, 1, 0))
		* glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1));

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(objectLoc,
		aiProcess_Triangulate |
		aiProcess_FlipUVs |
		aiProcess_GenSmoothNormals |
		aiProcess_JoinIdenticalVertices);

	if (!scene)
	{
		printf("Cannot load model %s: %s\n", objectLoc.c_str(), importer.GetErrorString());
		return;
	}

	aiNode* rootNode = scene->mRootNode;
	globalInverseTransform = aiMatrix4x4ToGlm(rootNode->mTransformation.Inverse());
	loadNode(rootNode, scene);
	loadTextures(scene);
}

void Model::loadNode(aiNode* node, const aiScene* scene)
{
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		loadModel(scene->mMeshes[node->mMeshes[i]], scene);
	}
	for (int i = 0; i < node->mNumChildren; i++)
	{
		loadNode(node->mChildren[i], scene);
	}
}

void Model::loadModel(aiMesh* mesh, const aiScene* scene)
{
	std::vector<GLfloat> vertex;
	std::vector<unsigned int> indices;
	std::vector<VertexBoneData> bones;
	bones.resize(mesh->mNumVertices);

	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		vertex.push_back(mesh->mVertices[i].x);
		vertex.push_back(mesh->mVertices[i].y);
		vertex.push_back(mesh->mVertices[i].z);

		if (mesh->mTextureCoords[0])
		{
			vertex.push_back(mesh->mTextureCoords[0][i].x);
			vertex.push_back(mesh->mTextureCoords[0][i].y);
		}
		else
		{
			vertex.push_back(0.0f);
			vertex.push_back(0.0f);
		}

		if (mesh->mColors[0])
		{
			vertex.push_back(mesh->mColors[0][i].r);
			vertex.push_back(mesh->mColors[0][i].g);
			vertex.push_back(mesh->mColors[0][i].b);
		}
		else
		{
			vertex.push_back(0.0f);
			vertex.push_back(0.0f);
			vertex.push_back(0.0f);
		}

		if (mesh->HasNormals())
		{
			vertex.push_back(mesh->mNormals[i].x);
			vertex.push_back(mesh->mNormals[i].y);
			vertex.push_back(mesh->mNormals[i].z);
		}
		else
		{
			vertex.push_back(0.0f);
			vertex.push_back(0.0f);
			vertex.push_back(0.0f);
		}

		if (mesh->HasTangentsAndBitangents())
		{
			vertex.push_back(mesh->mTangents[i].x);
			vertex.push_back(mesh->mTangents[i].y);
			vertex.push_back(mesh->mTangents[i].z);
		}
		else
		{
			vertex.push_back(0.0f);
			vertex.push_back(0.0f);
			vertex.push_back(0.0f);
		}
	}

	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	for (int i = 0; i < mesh->mNumBones; i++)
	{
		GLuint index = 0;
		std::string boneName(mesh->mBones[i]->mName.data);

		if (boneMapping.find(boneName) == boneMapping.end())
		{
			index = boneCount++;
			boneMapping[boneName] = index;
		}
		else
		{
			index = boneMapping[boneName];
		}

		glm::mat4 offsetMatrix = aiMatrix4x4ToGlm(mesh->mBones[i]->mOffsetMatrix);
		
		for (int j = 0; j < mesh->mBones[i]->mNumWeights; j++)
		{
			GLuint vertexId = mesh->mBones[i]->mWeights[j].mVertexId;
			GLfloat weight = mesh->mBones[i]->mWeights[j].mWeight;

			bones[vertexId].addData(boneCount, weight);
		}
	}

	std::vector<GLuint> bonesIds;
	std::vector<GLfloat> bonesWeights;

	for (int i = 0; i < bones.size(); i++)
	{
		for (int j = 0; j < BONES_PER_VERTEX; j++)
		{
			bonesIds.push_back(bones[i].ids[j]);
			bonesWeights.push_back(bones[i].weights[j]);
		}
	}

	Mesh* newMesh = new Mesh(&vertex[0],
		&indices[0],
		&bonesIds[0],
		&bonesWeights[0],
		vertex.size(),
		indices.size(),
		bonesIds.size());
	meshes.push_back(newMesh);
	// Map the texture to the mesh
	meshToTex.push_back(mesh->mMaterialIndex);
}

void Model::loadTextures(const aiScene* scene)
{
	textures.resize(scene->mNumMaterials);

	for (int i = 0; i < scene->mNumMaterials; i++)
	{
		textures[i] = nullptr;

		// Get the current material
		aiMaterial* material = scene->mMaterials[i];
		// Material name
		aiString materialName;

		// Get if the loading has been success
		aiReturn ret = material->Get(AI_MATKEY_NAME, materialName);
		if (ret != AI_SUCCESS) materialName = "";

		// Diffuse maps
		int numTextures = material->GetTextureCount(aiTextureType_DIFFUSE);
		aiString textureName;

		if (numTextures)
		{
			// Get the file name of the texture
			ret = material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), textureName);

			std::string name = textureName.data;

			if (name.substr(name.length() - 3, name.length() - 1) == "tif")
			{
				name = name.substr(0, name.length() - 3) + "png";
			}
			std::string realName = "";
			int p = name.length() - 1;
			while (p >= 0 && name[p] != '/' && name[p] != '\\')
			{
				realName.push_back(name[p]);
				p--;
			}
			std::reverse(realName.begin(), realName.end());
			std::string location = "../Textures/";
			std::string textureFileName = location + realName;

			textures[i] = new Texture(textureFileName);
			/*textures[i] = new Texture("../Textures/white.png");*/
		}
		else
		{
			textures[i] = new Texture("../Textures/Alien-Animal-Base-Color.jpg");
		}
	}
}

void Model::boneTransform(const GLfloat& seconds, const aiScene* scene)
{
	glm::mat4 identity = glm::mat4(1.0f);

	GLfloat ticksPerSecond = scene->mAnimations[0]->mTicksPerSecond != 0 ? scene->mAnimations[0]->mTicksPerSecond : 25.0f;
	GLfloat timeInTicks = seconds * ticksPerSecond;
	GLfloat animationTime = fmod(timeInTicks, scene->mAnimations[0]->mDuration);

	bonesTransformations.resize(boneMapping.size());

	readNodeHeirarchy(animationTime, scene->mRootNode, identity, scene);
}

void Model::readNodeHeirarchy(const GLfloat& animationTime, const aiNode* pNode, const glm::mat4& parentTransform, const aiScene* scene)
{
	std::string nodeName(pNode->mName.data);

	const aiAnimation* pAnimation = scene->mAnimations[0];

	glm::mat4 nodeTransformation(aiMatrix4x4ToGlm(pNode->mTransformation));
}

void Model::uploadUniforms()
{
	uploadGeneralUniforms();
	uploadGeneralLightUniforms();
	uploadPointLightsUniforms();
	uploadSpotLightUniforms();
}

void Model::uploadGeneralUniforms()
{
	glUniformMatrix4fv(shader->getUniforms().modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(shader->getUniforms().viewLoc, 1, GL_FALSE, glm::value_ptr(camera->getViewMatrix()));
	glUniformMatrix4fv(shader->getUniforms().projLoc, 1, GL_FALSE, glm::value_ptr(camera->getProjectionMatrix()));
	glUniform3fv(shader->getUniforms().eyePositionLoc, 1, glm::value_ptr(camera->getPosition()));
	glUniform1i(shader->getUniforms().numPointLightsLoc, shader->getPointLightUniforms().size());
}

void Model::uploadGeneralLightUniforms()
{
	glUniform3fv(shader->getGeneralLightUniforms().colourLoc, 1, glm::value_ptr(shader->getPhongLight()->getDirectionalLight().colour));
	glUniform1f(shader->getGeneralLightUniforms().ambientIntensityLoc, shader->getPhongLight()->getDirectionalLight().ambientIntensity);
	glUniform3fv(shader->getGeneralLightUniforms().directionLoc, 1, glm::value_ptr(shader->getPhongLight()->getDirectionalLight().direction));
	glUniform1f(shader->getGeneralLightUniforms().diffuseIntensityLoc, shader->getPhongLight()->getDirectionalLight().diffuseIntensity);
	glUniform1f(shader->getGeneralLightUniforms().specularIntensity, shader->getPhongLight()->getSpecularLight().specularIntensity);
	glUniform1f(shader->getGeneralLightUniforms().specularPower, shader->getPhongLight()->getSpecularLight().specularPower);
}

void Model::uploadPointLightsUniforms()
{
	for (int i = 0; i < shader->getPointLightUniforms().size(); i++)
	{
		glUniform3fv(shader->getPointLightUniforms()[i].colourLoc, 1, glm::value_ptr(shader->getPhongLight()->getPointLights()[i].colour));
		glUniform1f(shader->getPointLightUniforms()[i].ambientIntensityLoc, shader->getPhongLight()->getPointLights()[i].ambientIntensity);
		glUniform1f(shader->getPointLightUniforms()[i].diffuseIntensityLoc, shader->getPhongLight()->getPointLights()[i].diffuseIntensity);
		glUniform3fv(shader->getPointLightUniforms()[i].positionLoc, 1, glm::value_ptr(shader->getPhongLight()->getPointLights()[i].position));
		glUniform1f(shader->getPointLightUniforms()[i].constantLoc, shader->getPhongLight()->getPointLights()[i].constant);
		glUniform1f(shader->getPointLightUniforms()[i].linearLoc, shader->getPhongLight()->getPointLights()[i].linear);
		glUniform1f(shader->getPointLightUniforms()[i].expLoc, shader->getPhongLight()->getPointLights()[i].exp);
	}
}

void Model::uploadSpotLightUniforms()
{
	glUniform3fv(shader->getSpotLightUniforms().colourLoc, 1, glm::value_ptr(shader->getPhongLight()->getSpotLight().colour));
	glUniform1f(shader->getSpotLightUniforms().ambientIntensityLoc, shader->getPhongLight()->getSpotLight().ambientIntensity);
	glUniform1f(shader->getSpotLightUniforms().diffuseIntensityLoc, shader->getPhongLight()->getSpotLight().diffuseIntensity);
	glUniform3fv(shader->getSpotLightUniforms().positionLoc, 1, glm::value_ptr(camera->getPosition()));
	glUniform1f(shader->getSpotLightUniforms().constantLoc, shader->getPhongLight()->getSpotLight().constant);
	glUniform1f(shader->getSpotLightUniforms().linearLoc, shader->getPhongLight()->getSpotLight().linear);
	glUniform1f(shader->getSpotLightUniforms().expLoc, shader->getPhongLight()->getSpotLight().exp);
	glUniform3fv(shader->getSpotLightUniforms().directionLoc, 1, glm::value_ptr(camera->getDirection()));
	glUniform1f(shader->getSpotLightUniforms().cutOffLoc, cosf(shader->getPhongLight()->getSpotLight().cutOff));
}

glm::mat3 Model::aiMatrix3x3ToGlm(const aiMatrix3x3& matrix)
{
	glm::mat3 result;

	result[0][0] = matrix.a1; result[0][1] = matrix.b1; result[0][2] = matrix.c1;
	result[1][0] = matrix.a2; result[1][1] = matrix.b2; result[1][2] = matrix.c2;
	result[2][0] = matrix.a3; result[2][1] = matrix.b3; result[2][2] = matrix.c3;

	return result;
}

glm::mat4 Model::aiMatrix4x4ToGlm(const aiMatrix4x4& matrix)
{
	glm::mat4 result;

	result[0][0] = matrix.a1; result[0][1] = matrix.b1; result[0][2] = matrix.c1; result[0][3] = matrix.d1;
	result[1][0] = matrix.a2; result[1][1] = matrix.b2; result[1][2] = matrix.c2; result[1][3] = matrix.d2;
	result[2][0] = matrix.a3; result[2][1] = matrix.b3; result[2][2] = matrix.c3; result[2][3] = matrix.d3;
	result[3][0] = matrix.a4; result[3][1] = matrix.b4; result[3][2] = matrix.c4; result[3][3] = matrix.d4;

	return result;
}
