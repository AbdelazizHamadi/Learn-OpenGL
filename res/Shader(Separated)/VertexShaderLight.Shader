#version 330 core

layout(location = 0) in vec4 aposition;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aTexCoord;

out vec4 vertexColor;
out vec2 TexCoord;
uniform mat4 transform;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 projMatrix;
uniform mat4 rotation;
uniform mat4 scale;




void main ()
{

		gl_Position = projection * view * model * aposition;
		//gl_Position = aposition;
		vertexColor = aColor;
		//TexCoord    = aTexCoord; 
		TexCoord = aTexCoord;
	
};
