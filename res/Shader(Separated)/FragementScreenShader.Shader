//#version 330 core
//out vec4 FragColor;
//
//in vec2 TexCoords;
//
//uniform sampler2D screenTexture;
//
//void main()
//{
//    vec3 col = texture(screenTexture, TexCoords).rgb;
//    FragColor = vec4(col, 1.0);
//} 

#version 330 core

layout(location = 0) out vec4 color;

in vec2 TexCoord;


uniform sampler2D screenTexture;


void main()
{

	//color = texture(screenTexture, TexCoord);
	//float average = (color.r + color.g + color.b) / 3.0;
	//color = vec4(average, average, average, 1.0);


	color = vec4(vec3(texture(screenTexture, TexCoord)), 1.0);
			


};