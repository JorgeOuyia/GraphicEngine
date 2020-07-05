#ifndef PHONGLIGHT_H
#define PHONGLIGHT_H

#include <GL/glew.h>
#include <glm/glm.hpp>

struct DirectionalLight
{
	glm::vec3 colour;
	GLfloat ambientIntensity;
	glm::vec3 direction;
	GLfloat diffuseIntensity;

	DirectionalLight(glm::vec3 colour, GLfloat ambientIntensity, 
		glm::vec3 direction, GLfloat diffuseIntensity)
	{
		this->colour = colour;
		this->ambientIntensity = ambientIntensity;

		this->direction = direction;
		this->diffuseIntensity = diffuseIntensity;
	}
};

struct SpecularLight
{
	GLfloat specularIntensity;
	GLfloat specularPower;

	SpecularLight(GLfloat specularIntensity, GLfloat specularPower)
	{
		this->specularIntensity = specularIntensity;
		this->specularPower = specularPower;
	}
};

class PhongLight
{
public:
	PhongLight(DirectionalLight dirLight, 
		SpecularLight specLight);

	inline DirectionalLight getDirectionalLight() { return dirLight; }
	inline SpecularLight getSpecularLight() { return specLight; }

	~PhongLight();
private:
	DirectionalLight dirLight;
	SpecularLight specLight;
};

#endif // !PHONGLIGHT_H