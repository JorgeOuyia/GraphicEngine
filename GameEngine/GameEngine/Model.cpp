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
	std::vector<glm::mat4> transforms;
	if (scene->HasAnimations())
	{
		boneTransform(glfwGetTime(), transforms);
	}
	else
	{
		for (int i = 0; i < MAX_BONES; i++)
		{
			transforms.push_back(glm::mat4(1.0f));
		}
	}
	uploadSkeletalUniforms(transforms);
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
	delete scene;
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

	model = glm::translate(glm::mat4(1.0f), position)
		* glm::scale(glm::mat4(1.0f), scale)
		* glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1, 0, 0))
		* glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0, 1, 0))
		* glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1));

	Assimp::Importer importer;

	scene = importer.ReadFile(objectLoc,
		aiProcess_Triangulate |
		aiProcess_FlipUVs |
		aiProcess_GenSmoothNormals |
		aiProcess_JoinIdenticalVertices);

	if (!scene)
	{
		printf("Cannot load model %s: %s\n", objectLoc.c_str(), importer.GetErrorString());
		return;
	}

	scene = importer.GetOrphanedScene();

	aiNode* rootNode = scene->mRootNode;
	globalInverseTransform = aiMatrix4x4ToGlm(rootNode->mTransformation.Inverse());
	loadNode(rootNode);
	loadTextures();
}

void Model::loadNode(aiNode* node)
{
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		loadModel(scene->mMeshes[node->mMeshes[i]]);
	}
	for (int i = 0; i < node->mNumChildren; i++)
	{
		loadNode(node->mChildren[i]);
	}
}

void Model::loadModel(aiMesh* mesh)
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

	if (mesh->HasBones())
	{
		for (int i = 0; i < mesh->mNumBones; i++)
		{
			GLuint index = 0;
			std::string boneName(mesh->mBones[i]->mName.data);

			if (boneMapping.find(boneName) == boneMapping.end())
			{
				index = boneCount++;
				BoneMatrix boneMatrix;
				boneMatrices.push_back(boneMatrix);
				boneMatrices[index].offset = aiMatrix4x4ToGlm(mesh->mBones[i]->mOffsetMatrix);
				boneMapping[boneName] = index;
			}
			else
			{
				index = boneMapping[boneName];
			}

			for (int j = 0; j < mesh->mBones[i]->mNumWeights; j++)
			{
				GLuint vertexId = mesh->mBones[i]->mWeights[j].mVertexId;
				GLfloat weight = mesh->mBones[i]->mWeights[j].mWeight;

				bones[vertexId].addData(index, weight);
			}
		}
	}
	else
	{
		for (int i = 0; i < mesh->mNumVertices; i++)
		{
			VertexBoneData vbd;
			for (int j = 0; j < BONES_PER_VERTEX; j++)
			{
				vbd.ids[j] = 0;
				vbd.weights[j] = 1;
			}
			bones[i] = vbd;
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

void Model::uploadSkeletalUniforms(const std::vector<glm::mat4>& transforms)
{
	for (int i = 0; i < transforms.size(); i++)
	{
		glUniformMatrix4fv(shader->getSkeletalUniforms()[i].bonesLoc, 1, GL_FALSE, glm::value_ptr(transforms[i]));
	}
}

void Model::boneTransform(const GLfloat& timeInSeconds, std::vector<glm::mat4>& transforms)
{
	glm::mat4 identity = glm::mat4(1.0f);

	GLfloat ticksPerSecond = scene->mAnimations[0]->mTicksPerSecond != 0 ? scene->mAnimations[0]->mTicksPerSecond : 25.0f;
	GLfloat timeInTicks = ticksPerSecond * timeInSeconds;
	GLfloat animationTime = fmod(timeInTicks, scene->mAnimations[0]->mDuration);

	readNodeHeirarchy(animationTime, scene->mRootNode, identity);

	transforms.resize(boneCount);
	for (int i = 0; i < boneCount; i++)
	{
		transforms[i] = boneMatrices[i].finalWorldTransformation;
	}
}

void Model::readNodeHeirarchy(const GLfloat& animationTime, const aiNode* pNode, const glm::mat4& parentTransform)
{
	std::string nodeName(pNode->mName.data);

	const aiAnimation* pAnimation = scene->mAnimations[0];

	glm::mat4 nodeTransformation(aiMatrix4x4ToGlm(pNode->mTransformation));

	const aiNodeAnim* pNodeAnim = findNodeAnim(pAnimation, nodeName);

	if (pNodeAnim)
	{
		glm::vec3 scaleVector = interpolateScale(animationTime, pNodeAnim);
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), scaleVector);

		aiQuaternion rotationQuat = interpolateRotation(animationTime, pNodeAnim);
		aiMatrix4x4 rotMat(rotationQuat.GetMatrix());
		glm::mat4 rotation = glm::mat4(aiMatrix4x4ToGlm(rotMat));

		glm::vec3 positionVector = interpolatePosition(animationTime, pNodeAnim);
		glm::mat4 position = glm::translate(glm::mat4(1.0f), positionVector);

		nodeTransformation = position * rotation * scale;
	}

	glm::mat4 globalTransformation = parentTransform * nodeTransformation;

	if (boneMapping.find(nodeName) != boneMapping.end())
	{
		GLuint boneIndex = boneMapping[nodeName];
		boneMatrices[boneIndex].finalWorldTransformation = globalInverseTransform * globalTransformation * boneMatrices[boneIndex].offset;
	}

	for (int i = 0; i < pNode->mNumChildren; i++)
	{
		readNodeHeirarchy(animationTime, pNode->mChildren[i], globalTransformation);
	}
}

const aiNodeAnim* Model::findNodeAnim(const aiAnimation* pAnimation, const std::string& nodeName)
{
	for (int i = 0; i < pAnimation->mNumChannels; i++)
	{
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];
		if (std::string(pNodeAnim->mNodeName.data) == nodeName)
		{
			return pNodeAnim;
		}
	}
	return NULL;
}

glm::vec3 Model::interpolateScale(const GLfloat& pAnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1) { return aiVector3DToGlm(pNodeAnim->mScalingKeys[0].mValue); }

	GLuint scaleIndex = findScale(pAnimationTime, pNodeAnim);
	GLuint nextScaleIndex = scaleIndex + 1;
	GLfloat deltaTime = (GLfloat)(pNodeAnim->mScalingKeys[nextScaleIndex].mTime - pNodeAnim->mScalingKeys[scaleIndex].mTime);
	GLfloat factor = (pAnimationTime - (GLfloat)pNodeAnim->mScalingKeys[scaleIndex].mTime) / deltaTime;
	glm::vec3 start = aiVector3DToGlm(pNodeAnim->mScalingKeys[scaleIndex].mValue);
	glm::vec3 end = aiVector3DToGlm(pNodeAnim->mScalingKeys[nextScaleIndex].mValue);
	glm::vec3 delta = end - start;

	return start + factor * delta;
}

aiQuaternion Model::interpolateRotation(const GLfloat& pAnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumRotationKeys == 1) { return pNodeAnim->mRotationKeys[0].mValue; }

	GLuint rotationIndex = findRotation(pAnimationTime, pNodeAnim);
	GLuint nextRotationIndex = rotationIndex + 1;
	GLfloat deltaTime = (GLfloat)(pNodeAnim->mRotationKeys[nextRotationIndex].mTime - pNodeAnim->mRotationKeys[rotationIndex].mTime);
	GLfloat factor = (pAnimationTime - (GLfloat)pNodeAnim->mRotationKeys[rotationIndex].mTime) / deltaTime;
	aiQuaternion start = pNodeAnim->mRotationKeys[rotationIndex].mValue;
	aiQuaternion end = pNodeAnim->mRotationKeys[nextRotationIndex].mValue;
	
	return nlerp(start.Normalize(), end.Normalize(), factor);
}

glm::vec3 Model::interpolatePosition(const GLfloat& pAnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1) { return aiVector3DToGlm(pNodeAnim->mPositionKeys[0].mValue); }

	GLuint positionIndex = findPosition(pAnimationTime, pNodeAnim);
	GLuint nextPositionIndex = positionIndex + 1;
	GLfloat deltaTime = (GLfloat)(pNodeAnim->mPositionKeys[nextPositionIndex].mTime - pNodeAnim->mPositionKeys[positionIndex].mTime);
	GLfloat factor = (pAnimationTime - (GLfloat)pNodeAnim->mPositionKeys[positionIndex].mTime) / deltaTime;
	glm::vec3 start = aiVector3DToGlm(pNodeAnim->mPositionKeys[positionIndex].mValue);
	glm::vec3 end = aiVector3DToGlm(pNodeAnim->mPositionKeys[nextPositionIndex].mValue);
	glm::vec3 delta = end - start;

	return start + factor * delta;
}

GLuint Model::findScale(const GLfloat& pAnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++)
	{
		if (pAnimationTime < (GLfloat)pNodeAnim->mScalingKeys[i + 1].mTime) { return i; }
	}
	return 0;
}

GLuint Model::findRotation(const GLfloat& pAnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
	{
		if (pAnimationTime < (GLfloat)pNodeAnim->mRotationKeys[i + 1].mTime) { return i; }
	}
	return 0;
}

GLuint Model::findPosition(const GLfloat& pAnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++)
	{
		if (pAnimationTime < pNodeAnim->mPositionKeys[i + 1].mTime) { return i; }
	}
	return 0;
}

glm::mat3 Model::aiMatrix3x3ToGlm(const aiMatrix3x3& matrix)
{
	glm::mat3 result;

	result[0].x = matrix.a1; result[0].y = matrix.b1; result[0].z = matrix.c1;
	result[1].x = matrix.a2; result[1].y = matrix.b2; result[1].z = matrix.c2;
	result[2].x = matrix.a3; result[2].y = matrix.b3; result[2].z = matrix.c3;

	return result;
}

glm::mat4 Model::aiMatrix4x4ToGlm(const aiMatrix4x4& matrix)
{
	glm::mat4 result;

	result[0].x = matrix.a1; result[0].y = matrix.b1; result[0].z = matrix.c1; result[0].w = matrix.d1;
	result[1].x = matrix.a2; result[1].y = matrix.b2; result[1].z = matrix.c2; result[1].w = matrix.d2;
	result[2].x = matrix.a3; result[2].y = matrix.b3; result[2].z = matrix.c3; result[2].w = matrix.d3;
	result[3].x = matrix.a4; result[3].y = matrix.b4; result[3].z = matrix.c4; result[3].w = matrix.d4;

	return result;
}

glm::vec3 Model::aiVector3DToGlm(const aiVector3D& vector)
{
	glm::vec3 result;

	result.x = vector.x; result.y = vector.y; result.z = vector.z;

	return result;
}

aiQuaternion Model::nlerp(const aiQuaternion& a, const aiQuaternion& b, const GLfloat& blend)
{
	aiQuaternion result;
	GLfloat dotProduct = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	GLfloat oneMinusBlend = 1.0f - blend;

	if (dotProduct < 0.0f)
	{
		result.x = a.x * oneMinusBlend + blend * -b.x;
		result.y = a.y * oneMinusBlend + blend * -b.y;
		result.z = a.z * oneMinusBlend + blend * -b.z;
		result.w = a.w * oneMinusBlend + blend * -b.w;
	}
	else
	{
		result.x = a.x * oneMinusBlend + blend * b.x;
		result.y = a.y * oneMinusBlend + blend * b.y;
		result.z = a.z * oneMinusBlend + blend * b.z;
		result.w = a.w * oneMinusBlend + blend * b.w;
	}

	return result.Normalize();
}
