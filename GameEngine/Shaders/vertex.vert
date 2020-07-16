#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 colour;
layout(location = 3) in vec3 normal;
layout(location = 4) in vec3 tangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 fragmentPos;
out vec2 fragmentTex;
out vec3 fragmentNormal;

void main()
{	
	gl_Position = proj * view * model * vec4(pos, 1.0f);
	
	fragmentTex = tex;
	fragmentNormal = (transpose(inverse(model)) * vec4(normal, 1.0f)).xyz;
	fragmentPos = (model * vec4(pos, 1.0f)).xyz;
}