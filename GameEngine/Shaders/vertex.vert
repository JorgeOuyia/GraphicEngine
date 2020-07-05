#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 colour;
layout(location = 3) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform vec3 eye;

out vec3 fragmentPos;
out vec2 fragmentTex;
out vec3 fragmentNormal;
out vec3 fragmentEye;

void main()
{
	gl_Position = proj * view * model * vec4(pos, 1.0f);
	
	fragmentTex = tex;
	fragmentNormal = (view * model * vec4(normal, 0.0f)).xyz;
	fragmentEye = eye;
	fragmentPos = (model * vec4(pos, 0.0f)).xyz;
}