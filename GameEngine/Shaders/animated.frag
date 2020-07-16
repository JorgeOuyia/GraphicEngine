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

struct SpotLight
{
	vec3 colour;
	float ambientIntensity;
	float diffuseIntensity;
	vec3 position;
	float constant;
	float linear;
	float exp;
	vec3 direction;
	float cutOff;
};

in vec2 fragmentTex;
in vec3 fragmentNormal;
in vec3 fragmentPos;

out vec4 colour;

const int MAX_POINT_LIGHTS = 10;

uniform int numPointLights;
uniform DirectionalLight directionalLight;
uniform SpecularLight specularLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform vec3 eye;

uniform sampler2D tex;

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
		
		vec3 vertexToEye = normalize(eye - fragmentPos);
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
	SpecularLight specLight;
	specLight.specularIntensity = 0.0f;
	specLight.specularPower = 0.0f;
	vec4 colour = calcPhongLight(dirLight, specLight);
	
	float attenuation = pointLight.constant + (pointLight.linear * distance) + (pointLight.exp * distance * distance);
	
	return colour / attenuation;
}

vec4 calcSpotLight(SpotLight spotLight)
{
	vec3 lightToPixel = normalize(fragmentPos - spotLight.position);
	float spotFactor = dot(lightToPixel, spotLight.direction);
	
	if (spotFactor > spotLight.cutOff)
	{
		PointLight pLight;
		pLight.colour = spotLight.colour;
		pLight.ambientIntensity = spotLight.ambientIntensity;
		pLight.diffuseIntensity = spotLight.diffuseIntensity;
		pLight.position = spotLight.position;
		pLight.constant = spotLight.constant;
		pLight.linear = spotLight.linear;
		pLight.exp = spotLight.exp;
		vec4 colour = calcPointLight(pLight);
		
		return colour * (1.0f - (1.0f - spotFactor) * (1.0f / (1.0f - spotLight.cutOff)));
	}
	else
	{
		return vec4(0.0f, 0.0f, 0.0f, 0.0f);
	}
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

void main()
{
	//colour = texture(tex, fragmentTex) * (calcPhongLight(directionalLight, specularLight) + calcPointLights(pointLights) + calcSpotLight(spotLight));
	colour = vec4(fragmentNormal, 1.0f);
}