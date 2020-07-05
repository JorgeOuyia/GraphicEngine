#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <string>

#include "stb_image.h"

class Texture
{
public:
	Texture(std::string fileLoc);

	void use(const int &textureUnit);
	void unuse();

	~Texture();
private:
	GLuint textureID;
	std::string fileLoc;
	int width, height, bitDepth;

	unsigned char* readFile(const std::string &fileLoc);
	void init();
};

#endif // !TEXTURE_H