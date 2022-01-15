//#version 330 core
//layout (location = 0) in vec2 aPos;
//layout (location = 1) in vec2 aTexCoords;
//
//out vec2 TexCoords;
//
//void main()
//{
//    TexCoords = aTexCoords;
//    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
//}  

#version 330 core

layout(location = 0) in vec4 aposition;
layout(location = 2) in vec2 aTexCoord;

out vec2 TexCoord;


void main()
{
	TexCoord = aTexCoord;
	gl_Position = vec4(aposition.x, aposition.y, 0.0, 1.0);
	
};
