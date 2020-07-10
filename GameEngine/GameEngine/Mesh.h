#ifndef MESH_H
#define MESH_H

#include <unordered_map>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cstdio>
#include <cstdlib>

class Mesh
{
public:
	Mesh(GLfloat* vertex,
		unsigned int* indices,
		unsigned int vertexCount,
		unsigned int indexCount);
	Mesh(GLfloat* vertex,
		unsigned int* indices,
		GLuint* bonesIds,
		GLfloat* bonesWeights,
		unsigned int vertexCount,
		unsigned int indexCount,
		unsigned int boneCount);

	void render();

	~Mesh();
private:
	GLuint vao, ibo, vbo, bbo[2];

	GLfloat* vertex;
	unsigned int* indices;
	GLuint* bonesIds;
	GLfloat* bonesWeights;

	unsigned int vertexCount, indexCount, boneCount;

	void init();
};

#endif // !MESH_H