#ifndef MESH_H
#define MESH_H

#include <unordered_map>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Mesh
{
public:
	Mesh(GLfloat* vertex,
		unsigned int* indices,
		unsigned int vertexCount,
		unsigned int indexCount);

	void render();

	~Mesh();
private:
	GLuint vao, ibo, vbo;
	GLfloat* vertex;
	unsigned int* indices;
	unsigned int vertexCount, indexCount;

	void init();
};

#endif // !MESH_H