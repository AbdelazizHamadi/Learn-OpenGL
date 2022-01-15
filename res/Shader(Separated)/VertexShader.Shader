#version 330 core

layout(location = 0) in vec4 aposition;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aNormal;

out vec3 vertexColor;
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 transform;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 projMatrix;
uniform mat4 rotation;

void main ()
{

		gl_Position = projection * view * model * aposition;

		FragPos = vec3(model * aposition);
		
		vertexColor = aColor;
		
		TexCoord = aTexCoord;
		Normal = aNormal;
};
