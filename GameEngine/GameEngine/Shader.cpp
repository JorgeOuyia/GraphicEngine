#include "Shader.h"

Shader::Shader(std::string vertexLoc, std::string fragmentLoc, DirectionalLight directionalLight, SpecularLight specularLight, SpotLight spotLight) : program(0), vertexLoc(vertexLoc), fragmentLoc(fragmentLoc) 
{
	uniforms.modelLoc = 0;
	uniforms.viewLoc = 0;
	uniforms.projLoc = 0;
	uniforms.eyePositionLoc = 0;

	generalLightUniforms.ambientIntensityLoc = 0;
	generalLightUniforms.colourLoc = 0;
	generalLightUniforms.directionLoc = 0;
	generalLightUniforms.diffuseIntensityLoc = 0;
	generalLightUniforms.specularIntensity = 0;
	generalLightUniforms.specularPower = 0;

	phongLight = new PhongLight(directionalLight, specularLight, spotLight);
	init();
}

Shader::~Shader()
{
	if (program != 0)
	{
		glDeleteProgram(program);
		program = 0;
	}
}

void Shader::init()
{
	program = glCreateProgram();

	if (!program)
	{
		printf("Cannot create program\n");
		return;
	}

	std::string vertexSource = readFile(vertexLoc);
	std::string fragmentSource = readFile(fragmentLoc);

	attachShader(vertexSource.c_str(), GL_VERTEX_SHADER);
	attachShader(fragmentSource.c_str(), GL_FRAGMENT_SHADER);

	glLinkProgram(program);

	GLchar eLog[1024];
	GLint result;
	glGetProgramiv(program, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(program, sizeof(eLog), NULL, eLog);
		printf("Error linking program: %s\n", eLog);
		return;
	}
	glGetProgramiv(program, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(program, sizeof(eLog), NULL, eLog);
		printf("Error validating program: %s\n", eLog);
		return;
	}

	initUniforms();
}

std::string Shader::readFile(const std::string &fileLoc)
{
	std::ifstream file;
	file.open(fileLoc);
	if (!file.good())
	{
		printf("Cannot open file %s\n", fileLoc.c_str());
		return "";
	}

	std::string result = "";
	std::string line;
	while (std::getline(file, line))
	{
		result.append(line + "\n");
	}
	return result;
}

void Shader::attachShader(const char* shaderSource, GLenum shaderType)
{
	GLuint shader = glCreateShader(shaderType);

	if (!shader)
	{
		printf("Cannot create the shader\n");
		return;
	}

	const GLchar* theCode[1];
	theCode[0] = shaderSource;

	GLint theLength[1];
	theLength[0] = strlen(shaderSource);

	glShaderSource(shader, 1, theCode, theLength);
	glCompileShader(shader);

	GLchar eLog[1024];
	GLint result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error compiling %s shader: %s\n", shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment", eLog);
		return;
	}

	glAttachShader(program, shader);
}

void Shader::initUniforms()
{
	initGeneralUniforms();
	initGeneralLightUniforms();
	initPointLightsUniforms();
	initSpotLightUniforms();
}

void Shader::initGeneralUniforms()
{
	uniforms.modelLoc = glGetUniformLocation(program, "model");
	uniforms.viewLoc = glGetUniformLocation(program, "view");
	uniforms.projLoc = glGetUniformLocation(program, "proj");
	uniforms.eyePositionLoc = glGetUniformLocation(program, "eye");
	uniforms.numPointLightsLoc = glGetUniformLocation(program, "numPointLights");
}

void Shader::initGeneralLightUniforms()
{
	generalLightUniforms.colourLoc = glGetUniformLocation(program, "directionalLight.colour");
	generalLightUniforms.ambientIntensityLoc = glGetUniformLocation(program, "directionalLight.ambientIntensity");
	generalLightUniforms.directionLoc = glGetUniformLocation(program, "directionalLight.direction");
	generalLightUniforms.diffuseIntensityLoc = glGetUniformLocation(program, "directionalLight.diffuseIntensity");
	generalLightUniforms.specularIntensity = glGetUniformLocation(program, "specularLight.specularIntensity");
	generalLightUniforms.specularPower = glGetUniformLocation(program, "specularLight.specularPower");
}

void Shader::initPointLightsUniforms()
{
	for (int i = 0; i < phongLight->getPointLights().size(); i++)
	{
		std::string location;

		location.clear();
		location.append("pointLights[" + std::to_string(i)); location.append("].colour");
		pointLightUniforms[i].colourLoc = glGetUniformLocation(program, location.c_str());

		location.clear();
		location.append("pointLights[" + std::to_string(i)); location.append("].ambientIntensity");
		pointLightUniforms[i].ambientIntensityLoc = glGetUniformLocation(program, location.c_str());

		location.clear();
		location.append("pointLights[" + std::to_string(i)); location.append("].diffuseIntensity");
		pointLightUniforms[i].diffuseIntensityLoc = glGetUniformLocation(program, location.c_str());

		location.clear();
		location.append("pointLights[" + std::to_string(i)); location.append("].position");
		pointLightUniforms[i].positionLoc = glGetUniformLocation(program, location.c_str());

		location.clear();
		location.append("pointLights[" + std::to_string(i)); location.append("].constant");
		pointLightUniforms[i].constantLoc = glGetUniformLocation(program, location.c_str());

		location.clear();
		location.append("pointLights[" + std::to_string(i)); location.append("].linear");
		pointLightUniforms[i].linearLoc = glGetUniformLocation(program, location.c_str());

		location.clear();
		location.append("pointLights[" + std::to_string(i)); location.append("].exp");
		pointLightUniforms[i].expLoc = glGetUniformLocation(program, location.c_str());
	}
}

void Shader::initSpotLightUniforms()
{
	spotLightUniforms.colourLoc = glGetUniformLocation(program, "spotLight.colour");
	spotLightUniforms.ambientIntensityLoc = glGetUniformLocation(program, "spotLight.ambientIntensity");
	spotLightUniforms.diffuseIntensityLoc = glGetUniformLocation(program, "spotLight.diffuseIntensity");
	spotLightUniforms.positionLoc = glGetUniformLocation(program, "spotLight.position");
	spotLightUniforms.constantLoc = glGetUniformLocation(program, "spotLight.constant");
	spotLightUniforms.linearLoc = glGetUniformLocation(program, "spotLight.linear");
	spotLightUniforms.expLoc = glGetUniformLocation(program, "spotLight.exp");
	spotLightUniforms.directionLoc = glGetUniformLocation(program, "spotLight.direction");
	spotLightUniforms.cutOffLoc = glGetUniformLocation(program, "spotLight.cutOff");
}
