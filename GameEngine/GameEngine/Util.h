#ifndef UTIL_H

#define UTIL_H

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <GL/glew.h>

inline glm::mat3 aiMatrix3x3ToGlm(const aiMatrix3x3& matrix)
{
	glm::mat3 result;

	result[0].x = matrix.a1; result[0].y = matrix.b1; result[0].z = matrix.c1;
	result[1].x = matrix.a2; result[1].y = matrix.b2; result[1].z = matrix.c2;
	result[2].x = matrix.a3; result[2].y = matrix.b3; result[2].z = matrix.c3;

	return result;
}

inline glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& matrix)
{
	glm::mat4 result;

	result[0].x = matrix.a1; result[0].y = matrix.b1; result[0].z = matrix.c1; result[0].w = matrix.d1;
	result[1].x = matrix.a2; result[1].y = matrix.b2; result[1].z = matrix.c2; result[1].w = matrix.d2;
	result[2].x = matrix.a3; result[2].y = matrix.b3; result[2].z = matrix.c3; result[2].w = matrix.d3;
	result[3].x = matrix.a4; result[3].y = matrix.b4; result[3].z = matrix.c4; result[3].w = matrix.d4;

	return result;
}

inline glm::vec3 aiVector3DToGlm(const aiVector3D& vector)
{
	glm::vec3 result;

	result.x = vector.x; result.y = vector.y; result.z = vector.z;

	return result;
}

inline aiQuaternion nlerp(const aiQuaternion& a, const aiQuaternion& b, const GLfloat& blend)
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

#endif