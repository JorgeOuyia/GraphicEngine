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

in vec2 fragmentTex;
in vec3 fragmentNormal;
in vec3 fragmentEye;
in vec3 fragmentPos;

out vec4 colour;

uniform DirectionalLight directionalLight;
uniform SpecularLight specularLight;

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

uniform sampler2D tex;

void main()
{
	colour = texture(tex, fragmentTex) * calcPhongLight(directionalLight, specularLight);
	//colour = vec4(fragmentPos, 1.0f);
}