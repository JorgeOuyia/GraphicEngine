#include "Mesh.h"

Mesh::Mesh(GLfloat* vertex, unsigned int* indices, unsigned int vertexCount, unsigned int indexCount)
	: vao(0), ibo(0), vertex(vertex), indices(indices), vertexCount(vertexCount), indexCount(indexCount)
{
	bbo[0] = 0;
	bbo[1] = 0;
	init();
}

Mesh::Mesh(GLfloat* vertex, unsigned int* indices, GLuint* bonesIds, GLfloat* bonesWeights, unsigned int vertexCount, unsigned int indexCount, unsigned int boneCount)
	: vao(0), ibo(0), vertex(vertex), indices(indices), bonesIds(bonesIds), bonesWeights(bonesWeights), vertexCount(vertexCount), indexCount(indexCount), boneCount(boneCount)
{
	bbo[0] = 0;
	bbo[1] = 0;
	init();
}

void Mesh::render()
{
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Mesh::~Mesh()
{
	delete vertex;

	delete indices;

	if (vao != 0)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}
	if (ibo != 0)
	{
		glDeleteBuffers(1, &ibo);
		ibo = 0;
	}
	if (vbo != 0)
	{
		glDeleteBuffers(1, &vbo);
		vbo = 0;
	}
	if (bbo[0] != 0)
	{
		glDeleteBuffers(1, &bbo[0]);
		bbo[0] = 0;
	}
	if (bbo[1] != 0)
	{
		glDeleteBuffers(1, &bbo[1]);
		bbo[1] = 0;
	}
}

void Mesh::init()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indexCount, indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexCount, vertex, GL_STATIC_DRAW);

	// Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 14, 0);
	glEnableVertexAttribArray(0);
	// Texture
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 14, (void*)(sizeof(GLfloat) * 3));
	glEnableVertexAttribArray(1);
	// Colour
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 14, (void*)(sizeof(GLfloat) * 5));
	glEnableVertexAttribArray(2);
	// Normal
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 14, (void*)(sizeof(GLfloat) * 8));
	glEnableVertexAttribArray(3);
	// Tangents
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 14, (void*)(sizeof(GLfloat) * 11));
	glEnableVertexAttribArray(4);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Bone ids
	glGenBuffers(1, &bbo[0]);
	glBindBuffer(GL_ARRAY_BUFFER, bbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLuint) * boneCount, bonesIds, GL_STATIC_DRAW);

	glVertexAttribIPointer(5, 4, GL_INT, 0, 0);
	glEnableVertexAttribArray(5);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Bone weights
	glGenBuffers(1, &bbo[1]);
	glBindBuffer(GL_ARRAY_BUFFER, bbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * boneCount, bonesWeights, GL_STATIC_DRAW);

	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(6);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}