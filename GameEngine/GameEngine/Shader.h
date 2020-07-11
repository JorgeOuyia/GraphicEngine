#ifndef SHADER_H
#define SHADER_H

#define MAX_BONES 150

#include "PhongLight.h"

#include <string>
#include <fstream>
#include <sstream>
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct BaseLightUniforms
{
	GLuint colourLoc;
	GLuint ambientIntensityLoc;
	GLuint diffuseIntensityLoc;
};

struct Uniform
{
	GLuint modelLoc;
	GLuint viewLoc;
	GLuint projLoc;
	GLuint eyePositionLoc;
	GLuint numPointLightsLoc;
};

struct GeneralLightUniforms : public BaseLightUniforms
{
	GLuint directionLoc;
	GLuint specularIntensity;
	GLuint specularPower;
};

struct PointLightUniforms : public BaseLightUniforms
{
	GLuint positionLoc;
	GLuint constantLoc;
	GLuint linearLoc;
	GLuint expLoc;
};

struct SpotLightUniforms : public PointLightUniforms
{
	GLuint directionLoc;
	GLuint cutOffLoc;
};

struct SkeletalUniforms
{
	GLuint bonesLoc;
};

class Shader
{
public:
	Shader(std::string vertexLoc, std::string fragmentLoc,
		DirectionalLight directionalLight,
		SpecularLight specularLight,
		SpotLight spotLight);

	inline void use() { glUseProgram(program); }
	inline void unuse() { glUseProgram(0); }

	inline Uniform getUniforms() { return uniforms; }

	inline PhongLight* getPhongLight() { return phongLight; }
	inline GeneralLightUniforms getGeneralLightUniforms() { return generalLightUniforms; }
	inline SpotLightUniforms getSpotLightUniforms() { return spotLightUniforms; }
	inline std::vector<PointLightUniforms> getPointLightUniforms() { return pointLightUniforms; }
	inline std::vector<SkeletalUniforms> getSkeletalUniforms() const { return skeletalUniforms; }

	inline void addPointLight(const PointLight& pointLight) { phongLight->addPointLight(pointLight); pointLightUniforms.push_back(PointLightUniforms()), initUniforms(); }

	~Shader();
private:
	GLuint program;
	std::string vertexLoc, fragmentLoc;
	Uniform uniforms;
	GeneralLightUniforms generalLightUniforms;
	SpotLightUniforms spotLightUniforms;
	std::vector<PointLightUniforms> pointLightUniforms;
	std::vector<SkeletalUniforms> skeletalUniforms;
	PhongLight* phongLight;

	std::string readFile(const std::string &fileLoc);

	void init();
	void attachShader(const char* shaderSource, GLenum shaderType);

	void initUniforms();
	void initGeneralUniforms();
	void initGeneralLightUniforms();
	void initPointLightsUniforms();
	void initSpotLightUniforms();
	void initSkeletalUniforms();
};

#endif