#ifndef SHADER_H
#define SHADER_H

#include "PhongLight.h"

#include <string>
#include <fstream>
#include <sstream>
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Uniform
{
	GLuint modelLoc;
	GLuint viewLoc;
	GLuint projLoc;
	GLuint eyePositionLoc;
	GLuint numPointLightsLoc;
};

struct LightUniforms
{
	GLuint colourLoc;
	GLuint ambientIntensityLoc;
	GLuint directionLoc;
	GLuint diffuseIntensityLoc;
	GLuint specularIntensity;
	GLuint specularPower;
};

struct PointLightUniforms
{
	GLuint colourLoc;
	GLuint ambientIntensityLoc;
	GLuint diffuseIntensityLoc;
	GLuint positionLoc;
	GLuint constantLoc;
	GLuint linearLoc;
	GLuint expLoc;
};

class Shader
{
public:
	Shader(std::string vertexLoc, std::string fragmentLoc,
		DirectionalLight directionalLight,
		SpecularLight specularLight);

	inline void use() { glUseProgram(program); }
	inline void unuse() { glUseProgram(0); }

	inline Uniform getUniforms() { return uniforms; }

	inline PhongLight* getPhongLight() { return phongLight; }
	inline LightUniforms getLightUniforms() { return lightUniforms; }
	inline std::vector<PointLightUniforms> getPointLightUniforms() { return pointLightUniforms; }

	inline void addPointLight(const PointLight& pointLight) { phongLight->addPointLight(pointLight); pointLightUniforms.push_back(PointLightUniforms()), initUniforms(); }

	~Shader();
private:
	GLuint program;
	std::string vertexLoc, fragmentLoc;
	Uniform uniforms;
	LightUniforms lightUniforms;
	std::vector<PointLightUniforms> pointLightUniforms;
	PhongLight* phongLight;

	std::string readFile(const std::string &fileLoc);

	void init();
	void attachShader(const char* shaderSource, GLenum shaderType);

	void initUniforms();
};

#endif