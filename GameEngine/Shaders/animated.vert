#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 colour;
layout(location = 3) in vec3 normal;
layout(location = 4) in vec3 tangent;
layout(location = 5) in ivec4 boneId;
layout(location = 6) in vec4 boneWeight;

const int MAX_BONES = 100;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 bones[MAX_BONES];

out vec3 fragmentPos;
out vec2 fragmentTex;
out vec3 fragmentNormal;

void main()
{
	mat4 boneTransform = bones[boneId[0]] * boneWeight[0];
	boneTransform += bones[boneId[1]] * boneWeight[1];
	boneTransform += bones[boneId[2]] * boneWeight[2];
	boneTransform += bones[boneId[3]] * boneWeight[3];
	
	vec4 bonePos = boneTransform * vec4(pos, 1.0f);

	gl_Position = proj * view * model * bonePos;
	
	fragmentTex = tex;
	fragmentNormal = (transpose(inverse(model)) * bonePos).xyz;
	fragmentPos = (model * bonePos).xyz;
}