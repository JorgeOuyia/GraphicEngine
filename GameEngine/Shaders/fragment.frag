#version 330

struct DirectionalLight
{
	vec3 colour;
	float ambientIntensity;
	vec3 direction;
	float diffuseIntensity;
};

struct SpecularLight
{
	float specularIntensity;
	float specularPower;
};

struct PointLight
{
	vec3 colour;
	float ambientIntensity;
	float diffuseIntensity;
	vec3 position;
	float constant;
	float linear;
	float exp;
};

in vec2 fragmentTex;
in vec3 fragmentNormal;
in vec3 fragmentEye;
in vec3 fragmentPos;

out vec4 colour;

const int MAX_POINT_LIGHTS = 10;

uniform int numPointLights;
uniform DirectionalLight directionalLight;
uniform SpecularLight specularLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

vec4 calcPhongLight(DirectionalLight directionalLight, SpecularLight specularLight)
{
	vec4 ambientColour = vec4(directionalLight.colour * directionalLight.ambientIntensity, 1.0f);
	vec3 lightDirection = -directionalLight.direction;
	vec3 normal = normalize(fragmentNormal);
	
	float diffuseFactor = dot(normal, lightDirection);
	
	vec4 diffuseColour = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	vec4 specularColour = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	
	if (diffuseFactor > 0)
	{
		diffuseColour = vec4(directionalLight.colour, 1.0f) * directionalLight.diffuseIntensity * diffuseFactor;
		
		vec3 vertexToEye = normalize(fragmentEye - fragmentPos);
		vec3 lightReflect = normalize(reflect(directionalLight.direction, normal));
		float specularFactor = dot(vertexToEye, lightReflect);
		if (specularFactor > 0)
		{
			specularFactor = pow(specularFactor, specularLight.specularPower);
			specularColour = vec4(directionalLight.colour * specularLight.specularIntensity * specularFactor, 1.0f);
		}
	}
	return ambientColour + diffuseColour + specularColour;
}

vec4 calcPointLight(PointLight pointLight)
{
	vec3 lightDirection = fragmentPos - pointLight.position;
	float distance = length(lightDirection);
	lightDirection = normalize(lightDirection);
	
	DirectionalLight dirLight;
	dirLight.colour = pointLight.colour;
	dirLight.ambientIntensity = pointLight.ambientIntensity;
	dirLight.diffuseIntensity = pointLight.diffuseIntensity;
	dirLight.direction = lightDirection;
	SpecularLight specLight = specularLight;
	vec4 colour = calcPhongLight(dirLight, specLight);
	
	float attenuation = pointLight.constant + (pointLight.linear * distance) + (pointLight.exp * distance * distance);
	
	return colour / attenuation;
}

vec4 calcPointLights(PointLight pointLights[MAX_POINT_LIGHTS])
{
	vec4 colour = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	for (int i = 0; i < numPointLights; i++)
	{
		colour += calcPointLight(pointLights[i]);
	}
	return colour;
}

uniform sampler2D tex;

void main()
{
	colour = texture(tex, fragmentTex) * (calcPhongLight(directionalLight, specularLight) + calcPointLights(pointLights));
	//colour = vec4(fragmentPos, 1.0f);
}