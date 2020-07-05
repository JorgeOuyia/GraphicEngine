#include "Shader.h"

Shader::Shader(std::string vertexLoc, std::string fragmentLoc, DirectionalLight directionalLight, SpecularLight specularLight) : program(0), vertexLoc(vertexLoc), fragmentLoc(fragmentLoc) 
{
	uniforms.modelLoc = 0;
	uniforms.viewLoc = 0;
	uniforms.projLoc = 0;
	uniforms.eyePositionLoc = 0;

	lightUniforms.ambientIntensityLoc = 0;
	lightUniforms.colourLoc = 0;
	lightUniforms.directionLoc = 0;
	lightUniforms.diffuseIntensityLoc = 0;
	lightUniforms.specularIntensity = 0;
	lightUniforms.specularPower = 0;

	phongLight = new PhongLight(directionalLight, specularLight);
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
	uniforms.modelLoc = glGetUniformLocation(program, "model");
	uniforms.viewLoc = glGetUniformLocation(program, "view");
	uniforms.projLoc = glGetUniformLocation(program, "proj");
	uniforms.eyePositionLoc = glGetUniformLocation(program, "eye");

	lightUniforms.colourLoc = glGetUniformLocation(program, "directionalLight.colour");
	lightUniforms.ambientIntensityLoc = glGetUniformLocation(program, "directionalLight.ambientIntensity");
	lightUniforms.directionLoc = glGetUniformLocation(program, "directionalLight.direction");
	lightUniforms.diffuseIntensityLoc = glGetUniformLocation(program, "directionalLight.diffuseIntensity");

	lightUniforms.specularIntensity = glGetUniformLocation(program, "specularLight.specularIntensity");
	lightUniforms.specularPower = glGetUniformLocation(program, "specularLight.specularPower");
}