#include "Model.h"

Model::Model(std::string modelLoc, std::string vertexLoc, std::string fragmentLoc, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, Camera* camera)
	: modelLoc(modelLoc), vertexLoc(vertexLoc), fragmentLoc(fragmentLoc),
	position(position), scale(scale), rotation(rotation),
	camera(camera)
{
	init();
}

Model::~Model()
{
	importer.FreeScene();
	delete scene;
	delete shader;
	delete camera;
	for (Mesh* mesh : meshes) delete mesh;
	meshes.clear();
	for (Texture* texture : textures) delete texture;
	textures.clear();
	meshToTex.clear();
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

	calculateModel();

	scene = importer.ReadFile(modelLoc,
		aiProcess_Triangulate |
		aiProcess_FlipUVs |
		aiProcess_GenSmoothNormals |
		aiProcess_JoinIdenticalVertices);

	if (!scene)
	{
		printf("Cannot load model %s: %s\n", modelLoc.c_str(), importer.GetErrorString());
		return;
	}

	scene = importer.GetOrphanedScene();

	aiNode* rootNode = scene->mRootNode;
	globalInverseTransform = aiMatrix4x4ToGlm(rootNode->mTransformation.Inverse());
	loadTextures();
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

void Model::loadTextures()
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

			if (name.length() > 4)
			{
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
			}
			else
			{
				textures[i] = new Texture("../Textures/white.png");
			}
		}
		else
		{
			textures[i] = new Texture("../Textures/white.png");
		}
	}
}

void Model::loadGeneralModel(aiMesh* mesh, std::vector<GLfloat>& vertices, std::vector<unsigned int>& indices)
{
	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		vertices.push_back(mesh->mVertices[i].x);
		vertices.push_back(mesh->mVertices[i].y);
		vertices.push_back(mesh->mVertices[i].z);

		if (mesh->mTextureCoords[0])
		{
			vertices.push_back(mesh->mTextureCoords[0][i].x);
			vertices.push_back(mesh->mTextureCoords[0][i].y);
		}
		else
		{
			vertices.push_back(0.0f);
			vertices.push_back(0.0f);
		}

		if (mesh->mColors[0])
		{
			vertices.push_back(mesh->mColors[0][i].r);
			vertices.push_back(mesh->mColors[0][i].g);
			vertices.push_back(mesh->mColors[0][i].b);
		}
		else
		{
			vertices.push_back(0.0f);
			vertices.push_back(0.0f);
			vertices.push_back(0.0f);
		}

		if (mesh->HasNormals())
		{
			vertices.push_back(mesh->mNormals[i].x);
			vertices.push_back(mesh->mNormals[i].y);
			vertices.push_back(mesh->mNormals[i].z);
		}
		else
		{
			vertices.push_back(0.0f);
			vertices.push_back(0.0f);
			vertices.push_back(0.0f);
		}

		if (mesh->HasTangentsAndBitangents())
		{
			vertices.push_back(mesh->mTangents[i].x);
			vertices.push_back(mesh->mTangents[i].y);
			vertices.push_back(mesh->mTangents[i].z);
		}
		else
		{
			vertices.push_back(0.0f);
			vertices.push_back(0.0f);
			vertices.push_back(0.0f);
		}
	}

	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}
}

void Model::calculateModel()
{
	model = glm::translate(glm::mat4(1.0f), position)
		* glm::scale(glm::mat4(1.0f), scale)
		* glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1, 0, 0))
		* glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0, 1, 0))
		* glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1));
}

void Model::uploadUniforms()
{
	uploadGeneralUniforms();
	uploadGeneralLightUniforms();
	uploadPointLightsUniforms();
	uploadSpotLightUniforms();
}