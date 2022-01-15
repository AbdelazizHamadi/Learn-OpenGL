#shader vertex
#version 330 core

layout(location = 0) in vec4 aposition;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;

out vec3 vertexColor;
out vec2 TexCoord;
uniform mat4 transform;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 projMatrix;
uniform mat4 rotation;

void main ()
{

		gl_Position =  projMatrix  *  aposition;
		//gl_Position = aposition;
		vertexColor = aColor;
		//TexCoord    = aTexCoord; 
		TexCoord = aTexCoord;
	
};

#shader fragment 
#version 330 core

uniform vec2 u_resolution;
uniform float u_time;

layout(location = 0) out vec4 color;

uniform  vec4 u_color;

in vec3 vertexColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;

//uniform sampler2D ourTexture0;

void main ()
{

 
	//color = vec4(sin(u_time)/ vertexColor.x,cos(u_time)/vertexColor.y,sin(u_time)/ vertexColor.z,1.0);
	//color = texture(ourTexture,TexCoord) * vec4(sin(u_time) / vertexColor.x, cos(u_time) / vertexColor.y, sin(u_time) / vertexColor.z,0.2);
	 //color = vec4(vertexColor, 1.0);
	color = texture(ourTexture, TexCoord);
};