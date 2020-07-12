#include "AnimatedModel.h"

AnimatedModel::AnimatedModel(std::string modelLoc, std::string vertexLoc, std::string fragmentLoc, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, Camera* camera)
	: boneCount(0), Model(modelLoc, vertexLoc, fragmentLoc, position, scale, rotation, camera)
{
	loadNode(getScene()->mRootNode);
}

void AnimatedModel::render()
{
	activateShader();
	uploadUniforms();
	std::vector<glm::mat4> transforms;
	if (getScene()->HasAnimations())
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
	for (Mesh* mesh : getMeshes())
	{
		if (getTextureIndex(i) < getTextures().size() && getTexture(getTextureIndex(i))) { activateTexture(getTextureIndex(i), 0); }

		mesh->render();

		if (getTextureIndex(i) < getTextures().size() && getTexture(getTextureIndex(i))) { deactivateTexture(getTextureIndex(i)); }

		i++;
	}
	deactivateShader();
}

AnimatedModel::~AnimatedModel()
{
	boneMapping.clear();
	boneMatrices.clear();
	boneCount = 0;
}

void AnimatedModel::uploadSkeletalUniforms(const std::vector<glm::mat4>& transforms)
{
	for (int i = 0; i < transforms.size(); i++)
	{
		glUniformMatrix4fv(getShader()->getSkeletalUniforms()[i].bonesLoc, 1, GL_FALSE, glm::value_ptr(transforms[i]));
	}
}

void AnimatedModel::boneTransform(const GLfloat& timeInSeconds, std::vector<glm::mat4>& transforms)
{
	glm::mat4 identity = glm::mat4(1.0f);

	GLfloat ticksPerSecond = getScene()->mAnimations[0]->mTicksPerSecond != 0 ? getScene()->mAnimations[0]->mTicksPerSecond : 25.0f;
	GLfloat timeInTicks = ticksPerSecond * timeInSeconds;
	GLfloat animationTime = fmod(timeInTicks, getScene()->mAnimations[0]->mDuration);

	readNodeHeirarchy(animationTime, getScene()->mRootNode, identity);

	transforms.resize(boneCount);
	for (int i = 0; i < boneCount; i++)
	{
		transforms[i] = boneMatrices[i].finalWorldTransformation;
	}
}

void AnimatedModel::readNodeHeirarchy(const GLfloat& animationTime, const aiNode* pNode, const glm::mat4& parentTransform)
{
	std::string nodeName(pNode->mName.data);

	const aiAnimation* pAnimation = getScene()->mAnimations[0];

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
		boneMatrices[boneIndex].finalWorldTransformation = getGlobalInverseTransform() * globalTransformation * boneMatrices[boneIndex].offset;
	}

	for (int i = 0; i < pNode->mNumChildren; i++)
	{
		readNodeHeirarchy(animationTime, pNode->mChildren[i], globalTransformation);
	}
}

const aiNodeAnim* AnimatedModel::findNodeAnim(const aiAnimation* pAnimation, const std::string& nodeName)
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

glm::vec3 AnimatedModel::interpolateScale(const GLfloat& pAnimationTime, const aiNodeAnim* pNodeAnim)
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

aiQuaternion AnimatedModel::interpolateRotation(const GLfloat& pAnimationTime, const aiNodeAnim* pNodeAnim)
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

glm::vec3 AnimatedModel::interpolatePosition(const GLfloat& pAnimationTime, const aiNodeAnim* pNodeAnim)
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

GLuint AnimatedModel::findScale(const GLfloat& pAnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++)
	{
		if (pAnimationTime < (GLfloat)pNodeAnim->mScalingKeys[i + 1].mTime) { return i; }
	}
	return 0;
}

GLuint AnimatedModel::findRotation(const GLfloat& pAnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
	{
		if (pAnimationTime < (GLfloat)pNodeAnim->mRotationKeys[i + 1].mTime) { return i; }
	}
	return 0;
}

GLuint AnimatedModel::findPosition(const GLfloat& pAnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++)
	{
		if (pAnimationTime < pNodeAnim->mPositionKeys[i + 1].mTime) { return i; }
	}
	return 0;
}

void AnimatedModel::loadNode(aiNode* node)
{
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		loadModel(getScene()->mMeshes[node->mMeshes[i]]);
	}
	for (int i = 0; i < node->mNumChildren; i++)
	{
		loadNode(node->mChildren[i]);
	}
}

void AnimatedModel::loadModel(aiMesh* mesh)
{
	std::vector<GLfloat> vertices;
	std::vector<unsigned int> indices;
	std::vector<VertexBoneData> bones(mesh->mNumVertices);

	loadGeneralModel(mesh, vertices, indices);

	if (mesh->HasBones())
	{
		for (int i = 0; i < mesh->mNumBones; i++)
		{
			GLuint boneIndex = 0;
			std::string boneName(mesh->mBones[i]->mName.data);

			if (boneMapping.find(boneName) == boneMapping.end())
			{
				boneIndex = boneCount++;
				BoneMatrix boneMatrix(aiMatrix4x4ToGlm(mesh->mBones[i]->mOffsetMatrix), glm::mat4(1.0f));
				boneMatrices.push_back(boneMatrix);
				boneMapping[boneName] = boneIndex;
			}
			else
			{
				boneIndex = boneMapping[boneName];
			}

			for (int j = 0; j < mesh->mBones[i]->mNumWeights; j++)
			{
				GLuint vertexId = mesh->mBones[i]->mWeights[j].mVertexId;
				GLfloat vertexWeight = mesh->mBones[i]->mWeights[j].mWeight;

				bones[vertexId].addData(boneIndex, vertexWeight);
			}
		}
	}
	else
	{
		for (int i = 0; i < mesh->mNumVertices; i++)
		{
			VertexBoneData vertexBoneData;
			for (int j = 0; j < BONES_PER_VERTEX; j++)
			{
				vertexBoneData.ids[j] = 0u;
				vertexBoneData.weights[j] = 1.0f;
			}
			bones[i] = vertexBoneData;
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

	addMesh(new Mesh(&vertices[0], &indices[0], &bonesIds[0], &bonesWeights[0], vertices.size(), indices.size(), bonesIds.size()));
	addMeshToTex(mesh->mMaterialIndex);
}
