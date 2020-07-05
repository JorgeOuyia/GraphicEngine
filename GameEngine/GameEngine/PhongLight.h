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

struct SpotLight : public PointLight
{
	glm::vec3 direction;
	GLfloat cutOff;

	SpotLight(glm::vec3 colour, GLfloat ambientIntensity, GLfloat diffuseIntensity,
		glm::vec3 position, GLfloat constant, GLfloat linear, GLfloat exp,
		glm::vec3 direction, GLfloat cutOff) : PointLight(colour, ambientIntensity, diffuseIntensity,
			position, constant, linear, exp)
	{
		this->direction = direction;
		this->cutOff = cutOff;
	}
};

class PhongLight
{
public:
	PhongLight(DirectionalLight dirLight,
		SpecularLight specLight,
		SpotLight spotLight);

	inline DirectionalLight getDirectionalLight() { return dirLight; }
	inline SpecularLight getSpecularLight() { return specLight; }
	inline SpotLight getSpotLight() { return spotLight; }
	inline std::vector<PointLight> getPointLights() { return pointLights; }

	inline void addPointLight(const PointLight& pointLight) { pointLights.push_back(pointLight); }

	~PhongLight();
private:
	DirectionalLight dirLight;
	SpecularLight specLight;
	SpotLight spotLight;
	std::vector<PointLight> pointLights;
};

#endif // !PHONGLIGHT_H