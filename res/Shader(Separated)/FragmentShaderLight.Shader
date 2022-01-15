#version 330 core

uniform vec2 u_resolution;
uniform float u_time;

layout(location = 0) out vec4 color;

uniform  vec4 u_color;
uniform vec3 objectColor;
uniform vec3 lightColor;


in vec3 vertexColor;
in vec2 TexCoord;
//uniform sampler2D ourTexture0;

void main()
{


	//color = vec4(sin(u_time)/ vertexColor.x,cos(u_time)/vertexColor.y,sin(u_time)/ vertexColor.z,1.0);
	//color = texture(ourTexture,TexCoord) * vec4(sin(u_time) / vertexColor.x, cos(u_time) / vertexColor.y, sin(u_time) / vertexColor.z,0.2);
	color = vec4(objectColor, 1.0);
	//color = texture(ourTexture, TexCoord);
	// color = vec4(lightColor * objectColor, 1.0);

};