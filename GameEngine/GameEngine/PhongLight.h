#ifndef PHONGLIGHT_H
#define PHONGLIGHT_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>

struct BaseLight
{
	glm::vec3 colour;
	GLfloat ambientIntensity;
	GLfloat diffuseIntensity;

	BaseLight(glm::vec3 colour, GLfloat ambientIntensity, GLfloat diffuseIntensity)
	{
		this->colour = colour;
		this->ambientIntensity = ambientIntensity;
		this->diffuseIntensity = diffuseIntensity;
	}
};

struct DirectionalLight : public BaseLight
{
	glm::vec3 direction;

	DirectionalLight(glm::vec3 colour, GLfloat ambientIntensity, GLfloat diffuseIntensity, glm::vec3 direction) : BaseLight(colour, ambientIntensity, diffuseIntensity)
	{
		this->direction = direction;
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

struct PointLight : public BaseLight
{
	glm::vec3 position;
	GLfloat constant;
	GLfloat linear;
	GLfloat exp;

	PointLight(glm::vec3 colour, GLfloat ambientIntensity, GLfloat diffuseIntensity,
		glm::vec3 position, GLfloat constant, GLfloat linear, GLfloat exp) : BaseLight(colour, ambientIntensity, diffuseIntensity)
	{
		this->position = position;
		this->constant = constant;
		this->linear = linear;
		this->exp = exp;
	}
};

class PhongLight
{
public:
	PhongLight(DirectionalLight dirLight,
		SpecularLight specLight);

	inline DirectionalLight getDirectionalLight() { return dirLight; }
	inline SpecularLight getSpecularLight() { return specLight; }
	inline std::vector<PointLight> getPointLights() { return pointLights; }
	
	inline void addPointLight(const PointLight& pointLight) { pointLights.push_back(pointLight); }

	~PhongLight();
private:
	DirectionalLight dirLight;
	SpecularLight specLight;
	std::vector<PointLight> pointLights;
};

#endif // !PHONGLIGHT_H