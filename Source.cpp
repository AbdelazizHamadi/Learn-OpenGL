#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GLFW/glfw3.h>
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<GL/glut.h>
#include "stb_image.h" 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp> 
#include <glm/gtx/string_cast.hpp>
#include <string>
#include <vector>

#include"Shader.h"
#include "Camera.h"
#include "Sphere.h"


// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

//light Tension
float lightTesnion = 0.5f;
float lightBrightness = 0.1f;
float resultLight = 1.0f;

// Camera
float camz = -0.5F;
float camView = -2.0f;
glm::vec3 cameraPos = glm::vec3(0.0f, -1.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

//Materiels 
struct Material {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};

//model Choice
int ModelChoice = 1;
int TextureActivation = 0;

//cook_torrance parametres 

float Roughness = 0;
float F0 = 0.0;
float k = 0.1;

//objectMaterial 
int ObjectMaterial = 0;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_E && action == GLFW_PRESS)
	{
		camz = camz + 1000.0f;
	}

	if (key == GLFW_KEY_K && action == GLFW_PRESS)
	{
		k = k + 0.1;
	}

	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		lightTesnion = lightTesnion - 0.05;
		resultLight = resultLight - 0.1;

	}
	if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		lightTesnion = lightTesnion + 0.05;
		resultLight = resultLight + 0.1;

	}

	//Texture Activation 
	if (key == GLFW_KEY_T && action == GLFW_PRESS)
	{

		TextureActivation++;
		TextureActivation = TextureActivation % 2;

	}

	//phong Shading 
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		ModelChoice = 1;

	}
	//blinn-phong Shading
	if (key == GLFW_KEY_B && action == GLFW_PRESS)
	{
		ModelChoice = 2;

	}

	//blinn-phong Shading
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		ModelChoice = 4;

	}

	if (key == GLFW_KEY_C && action == GLFW_PRESS)
	{
		ModelChoice = 3;

	}

	if (key == GLFW_KEY_G && action == GLFW_PRESS)
	{
		ModelChoice = 6;

	}

	if (key == GLFW_KEY_N && action == GLFW_PRESS)
	{
		ModelChoice = 5;

	}


	//change Material values

	if (key == GLFW_KEY_O && action == GLFW_PRESS)
	{
		ObjectMaterial++;
		ObjectMaterial = ObjectMaterial % 3;


	}

	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		Roughness += 0.001;

		std::cout << "" << std::endl;
		std::cout << "Roughness value : " << Roughness << std::endl;
		std::cout << "k value : " << k << std::endl;
		std::cout << "Fresnel F0 value : " << F0 << std::endl;
	}

	if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		F0 += 0.01;
		std::cout << "" << std::endl;
		std::cout << "Roughness value : " << Roughness << std::endl;
		std::cout << "k value : " << k << std::endl;
		std::cout << "Fresnel F0 value : " << F0 << std::endl;
	}

	if (key == GLFW_KEY_K && action == GLFW_PRESS)
	{
		k += 0.01;
		std::cout << "" << std::endl;
		std::cout << "Roughness value : " << Roughness << std::endl;
		std::cout << "k value : " << k << std::endl;
		std::cout << "Fresnel F0 value : " << F0 << std::endl;

	}
	const float cameraSpeed = 0.05f; // adjust accordingly
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

float Rotate = 0;

static void GLClearError() {

	while (!glGetError);

}

static void GLCheckError() {

	while (GLenum error = glGetError())
	{
		std::cout << "[openGl Error] (" << error << ")" << std::endl;
	}
}


struct ShaderProgamSource {

	std::string VertexSource;
	std::string FragmentSource;
};
static ShaderProgamSource ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);
	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;

	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				type = ShaderType::VERTEX;
			}
			else
				if (line.find("fragment") != std::string::npos)
				{
					type = ShaderType::FRAGMENT;
				}

		}
		else
		{
			ss[(int)type] << line << "\n";
		}
	}

	return { ss[0].str(), ss[1].str() };

}
static unsigned int CompileShader(unsigned int type, const std::string& source)

{

	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);

	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);

		std::cout << "fail to Compile " << (type == GL_VERTEX_SHADER ? "vertex " : "Fragnment ") << "shader !" << std::endl;
		std::cout << message << std::endl;

		glDeleteShader(id);
		return 0;

	}


	return id;

}


static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{

	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;

}


int main(void)
{


	GLFWwindow* window;


	/* Initialize the library */
	if (!glfwInit())
		return -1;


	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(960, 540, "Hello World", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)

		std::cout << "ERROR !" << std::endl;
	std::cout << glGetString(GL_VERSION) << std::endl;


	float positions[] = {


		//position				//colors			//texture		
		-0.5f, -0.5f, 0.5f,		1.0f, 0.0f, 0.0f,	0.0f,0.0f,					// 0
		-0.5f ,-0.5f,-0.5f, 	0.0f, 1.0f, 0.0f,	1.0f,0.0f,					// 1
		 0.5f ,-0.5f, 0.5f,  	0.0f, 0.0f, 1.0f,	1.0f,0.0f,					// 2	
		 0.5f ,-0.5f,-0.5f, 	1.0f, 1.0f, 1.0f,	0.0f,1.0f,					// 3	
		 0.5f , 0.5f, 0.5f, 	0.0f, 1.0f, 0.0f,	1.0f,1.0f,					// 4	
		 0.5f , 0.5f,-0.5f, 	1.0f, 1.0f, 0.5f,	0.5f,0.5f, 					// 5
		-0.5f , 0.5f, 0.5f, 	1.0f, 1.0f, 0.0f,	0.0f,1.0f,					// 6
		-0.5f , 0.5f,-0.5f, 	1.0f, 1.0f, 0.5f,	0.5f,0.5f					// 7

	};

	float vertices[] = {

		//positions				//colors			 //texture      //normals			 
		-0.5f, -0.5f, -0.5f,	1.0f, 0.0f, 0.0f,	 0.0f, 0.0f,    0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,	0.0f, 1.0f, 0.0f,	 1.0f, 0.0f,    0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,	0.0f, 0.0f, 1.0f,	 1.0f, 1.0f,    0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,	1.0f, 1.0f, 1.0f,	 1.0f, 1.0f,    0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,	0.0f, 1.0f, 0.0f,	 0.0f, 1.0f,    0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,	1.0f, 1.0f, 0.5f,	 0.0f, 0.0f,    0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,	1.0f, 0.0f, 0.0f,	 0.0f, 0.0f,    0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,	0.0f, 1.0f, 0.0f,	 1.0f, 0.0f,    0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,	0.0f, 0.0f, 1.0f,	 1.0f, 1.0f,    0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,	1.0f, 1.0f, 1.0f,	 1.0f, 1.0f,    0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,	0.0f, 1.0f, 0.0f,	 0.0f, 1.0f,    0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,	1.0f, 1.0f, 0.5f,	 0.0f, 0.0f,    0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f,	1.0f, 0.0f, 0.0f,	 1.0f, 0.0f,   -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,	0.0f, 1.0f, 0.0f,	 1.0f, 1.0f,   -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, 1.0f,	 0.0f, 1.0f,   -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,	1.0f, 1.0f, 1.0f,	 0.0f, 1.0f,   -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,	0.0f, 1.0f, 0.0f,	 0.0f, 0.0f,   -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,	1.0f, 1.0f, 0.5f,	 1.0f, 0.0f,   -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 0.0f,	 1.0f, 0.0f,    1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f,    1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 1.0f,    0.0f, 1.0f,    1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f,    1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f,    1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 0.5f,    1.0f, 0.0f,    1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,	 0.0f, 1.0f,    0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,    0.0f, 1.0f, 0.0f,	 1.0f, 1.0f,    0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,	 1.0f, 0.0f,    0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,    1.0f, 1.0f, 1.0f,	 1.0f, 0.0f,    0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,    0.0f, 1.0f, 0.0f,	 0.0f, 0.0f,    0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,    1.0f, 1.0f, 0.5f,	 0.0f, 1.0f,    0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,	1.0f, 0.0f, 0.0f,	 0.0f, 1.0f,    0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,	0.0f, 1.0f, 0.0f,	 1.0f, 1.0f,    0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,	0.0f, 0.0f, 1.0f,	 1.0f, 0.0f,    0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,	1.0f, 1.0f, 1.0f,	 1.0f, 0.0f,    0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,	0.0f, 1.0f, 0.0f,	 0.0f, 0.0f,    0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,	1.0f, 1.0f, 0.5f,	 0.0f, 1.0f,    0.0f,  1.0f,  0.0f
	};
	unsigned int indices[] = {

		//face1 

		0,2,4,
		4,6,0

		/*

		//face2

		2,3,5,4,

		//face3

		3,5,7,1,

		//face4

		0,1,7,6,

		//face5

		0,2,3,1,

		//face6

		6,4,5,7


		*/



	};


	//create a plane 

	float planeVertices[] = {
		// positions          // texture Coords 
		 5.0f, -2.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -2.5f,  5.0f,  0.0f, 0.0f,
		-5.0f, -2.5f, -5.0f,  0.0f, 2.0f,
				
		 5.0f, -2.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -2.5f, -5.0f,  0.0f, 2.0f,
		 5.0f, -2.5f, -5.0f,  2.0f, 2.0f
	};

	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	// lighting info
	// -------------

	const unsigned int NR_LIGHTS = 32;
	std::vector<glm::vec3> lightPositions;
	std::vector<glm::vec3> lightColors;
	srand(13);
	for (unsigned int i = 0; i < NR_LIGHTS; i++)
	{
		// calculate slightly random offsets
		float xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		float yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
		float zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
		// also calculate random color
		float rColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
		float gColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
		float bColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
		lightColors.push_back(glm::vec3(rColor, gColor, bColor));
	}

	// screen quad VAO
	unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));



	//plane VAO,VBO
	unsigned int planeVAO, planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	// framebuffer configuration
	// -------------------------
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	//gBuffers
	unsigned int gPosition, gNormal, gAlbedoSpec;
	
	// position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	// normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	// color + specular color buffer
	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);


	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	
	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);



	//// Texture parametres 
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	//float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	////loading a texture 

	int width, height, nrChannels;
	unsigned char* data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
	unsigned int texture;
	GLCheckError();

	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);


	stbi_image_free(data);

	unsigned int buffer, CubeVAO;
	glGenVertexArrays(1, &CubeVAO);
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(CubeVAO);

	//positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), 0);

	//colors
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));

	//texture
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//normals
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);


	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//load files (fragment and vertex shaders ) METHODE 1



	unsigned int lightVAO;

	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	// we only need to bind to the VBO, the container's VBO's data already contains the data.
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	//positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//drawing sphere


	Sphere sphere(1.0f, 36, 18, true);

	unsigned int SphereVAO;
	glGenVertexArrays(1, &SphereVAO);
	glBindVertexArray(SphereVAO);
	glBindBuffer(GL_ARRAY_BUFFER, SphereVAO);


	glBufferData(GL_ARRAY_BUFFER, sphere.getInterleavedVertexSize(), sphere.getInterleavedVertices(), GL_STATIC_DRAW);



	unsigned int SphereIBO;
	glGenBuffers(1, &SphereIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SphereIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere.getIndexSize(), sphere.getIndices(), GL_STATIC_DRAW);



	//positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sphere.getInterleavedStride(), (void*)0);


	//texture
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sphere.getInterleavedStride(), (void*)(6 * sizeof(float)));



	//normals
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sphere.getInterleavedStride(), (void*)(3 * sizeof(float)));





	/*

	ShaderProgamSource source = ParseShader("res/shader/shading.shader");

		std::cout << "Vertex" << std::endl;
		std::cout << source.VertexSource << std::endl;
		std::cout << "fragment" << std::endl;
		std::cout << source.FragmentSource << std::endl;


		unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
		glUseProgram(shader);

	*/



	//load files (fragment and vertex shaders )	METHODE 2

	Shader ourShader("res/Shader(Separated)/VertexShader.Shader", "res/Shader(Separated)/FragmentShader.Shader");
	ourShader.use();
	Shader lightingShader("res/Shader(Separated)/VertexShaderLight.Shader", "res/Shader(Separated)/FragmentShaderLight.Shader");
	Shader screenShader("res/Shader(Separated)/VertexScreenShader.Shader", "res/Shader(Separated)/FragementScreenShader.Shader");
	Shader shaderGeometryPass("res/Shader(Separated)/VertexGbuffer.Shader", "res/Shader(Separated)/FragementGbuffer.Shader");
	Shader shaderLightingPass("res/Shader(Separated)/deferred_shading_Vertex.Shader", "res/Shader(Separated)/deferred_shading_Fragement.Shader");

	
	screenShader.use();
	screenShader.setInt("screenTexture", 0);
	/*

	glm::vec3 objectcolor = glm::vec3 (1.0f, 0.5f, 0.31f);
	glm::vec3 lightColor = glm::vec3 (1.0,1.0,1.0);
	ourShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
	//ourShader.setVec3("lightColor", lightColor);


	*/






	glm::vec3 cubePositions[] = {
			glm::vec3(0.0f,  0.0f,  0.0f),
			glm::vec3(2.0f,  5.0f, -15.0f),
			glm::vec3(-1.5f, -2.2f, -2.5f),
			glm::vec3(-3.8f, -2.0f, -12.3f),
			glm::vec3(2.4f, -0.4f, -3.5f),
			glm::vec3(-1.7f,  3.0f, -7.5f),
			glm::vec3(1.3f, -2.0f, -2.5f),
			glm::vec3(1.5f,  2.0f, -2.5f),
			glm::vec3(1.5f,  0.2f, -1.5f),
			glm::vec3(-1.3f,  1.0f, -1.5f)
	};


	//window controls

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwSetKeyCallback(window, key_callback);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */


		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);

		glClearColor(0.878, 0.878, 0.878, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		glEnable(GL_DEPTH_TEST);


		float lightPosx = 1.0f + sin(glfwGetTime()) * 2.0f;
		float lightPosy = sin(glfwGetTime() / 2.0f) * 1.0f;
		float lightPosz = 2.0 + sin(glfwGetTime() / 2.0f) * 1.5f;

		GLClearError();

		// Enable depth test


		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


		//drawing the light 


		lightingShader.use();


		if (resultLight >= 1)
			resultLight = 1;
		if (resultLight <= 0)
			resultLight = 0;

		lightingShader.setVec3("objectColor", resultLight, resultLight, resultLight);

		glm::mat4 View = glm::mat4(1.0f);
		View = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);


		lightingShader.setMat4("view", View);

		glm::mat4 projection;
		projection = glm::perspective(70.0f, 4.0f / 3.0f, 0.1f, 100.0f);

		lightingShader.setMat4("projection", projection);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(lightPosx, lightPosy, lightPosz));

		glm::mat4 scale = glm::mat4(1.0f);
		scale = glm::scale(scale, glm::vec3(0.125f, 0.125f, 0.125f));

		glm::mat4 modelScale = glm::mat4(1.0f);
		modelScale = model * scale;

		lightingShader.setMat4("model", modelScale);



		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		//Drawing a cube


		ourShader.use();


		//	ourShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		ourShader.setVec3("lightColor", resultLight, resultLight, resultLight);
		ourShader.setVec3("lightPos", lightPosx, lightPosy, lightPosz);
		ourShader.setVec3("viewPos", cameraPos.x, cameraPos.y, cameraPos.y);

		View = glm::mat4(1.0f);
		View = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		ourShader.setMat4("view", View);

		projection = glm::perspective(70.0f, 4.0f / 3.0f, 0.1f, 100.0f);
		ourShader.setMat4("projection", projection);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, -2.0f, 0.0f));
		ourShader.setMat4("model", model);

		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(CubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);





		//drawing a Sphere


		if (ObjectMaterial == 0)
		{

			//object materials  (GOLD)
			ourShader.setVec3("objectColor", 1.000, 0.766, 0.336);
			ourShader.setVec3("Mambient", 0.24725, 0.1995, 0.0745);
			ourShader.setVec3("Mdiffuse", 0.75164, 0.60648, 0.22648);
			ourShader.setVec3("Mspecular", 0.628281, 0.555802, 0.366065);
			ourShader.setFloat("Mshininess", 51.2f);
			ourShader.setFloat("roughness", k); // 0.51
			ourShader.setFloat("F0", 0.8);
			ourShader.setFloat("k", 0.2);

		}


		if (ObjectMaterial == 1)
		{


			//objectMaterials (Metal)
			ourShader.setVec3("Mambient", 0.1745, 0.01175, 0.01175);
			ourShader.setVec3("Mdiffuse", 0.61424, 0.04136, 0.04136);
			ourShader.setVec3("Mspecular", 0.727811, 0.626959, 0.626959);
			ourShader.setFloat("Mshininess", 76.8);
			ourShader.setFloat("roughness", 0.42);
			ourShader.setFloat("F0", 0.18);
			ourShader.setFloat("k", 0.21 + 0.19 + 0.28 + k);
		}

		if (ObjectMaterial == 2)
		{


			//objectMaterials  (Bronze)
			ourShader.setVec3("Mambient", 0.2125, 0.1275, 0.054);
			ourShader.setVec3("Mdiffuse", 0.714, 0.4284, 0.18144);
			ourShader.setVec3("Mspecular", 0.393548, 0.271906, 0.166721);
			ourShader.setFloat("Mshininess", 25.6);
			ourShader.setFloat("roughness", 0.42); //0.42
			ourShader.setFloat("F0", 0.09 + 0.09);
			ourShader.setFloat("k", 0.21 + 0.19 + 0.28);
		}




		//light materials
		ourShader.setVec3("Ldiffuse", 1.0, 1.0, 1.0);
		ourShader.setVec3("Lspecular", 1.0, 1.0, 1.0);
		ourShader.setVec3("Lambient", 0.2f, 0.2f, 0.2f);

		//modelChoice 
		ourShader.setInt("ModelChoice", ModelChoice);

		//Texture State
		ourShader.setInt("TextureState", TextureActivation);

		ourShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		ourShader.setVec3("lightColor", resultLight, resultLight, resultLight);
		ourShader.setVec3("lightPos", lightPosx, lightPosy, 0);
		ourShader.setVec3("viewPos", 0.0, 0.0, 3.0);

		View = glm::mat4(1.0f);
		View = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		ourShader.setMat4("view", View);

		projection = glm::perspective(70.0f, 4.0f / 3.0f, 0.1f, 100.0f);
		ourShader.setMat4("projection", projection);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, -3.0f));
		ourShader.setMat4("model", model);

		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(SphereVAO);
		glDrawElements(GL_TRIANGLES, sphere.getIndexCount(), GL_UNSIGNED_INT, (void*)0);


		// floor

		glBindVertexArray(planeVAO);

		View = glm::mat4(1.0f);
		View = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		ourShader.setMat4("view", View);

		projection = glm::perspective(70.0f, 4.0f / 3.0f, 0.1f, 100.0f);
		ourShader.setMat4("projection", projection);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, -10.0f, -2.0f));
		ourShader.setMat4("model", model);

		ourShader.setMat4("model", glm::mat4(1.0f));

		glBindTexture(GL_TEXTURE_2D, texture);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);


		// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
		// clear all relevant buffers
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
		glClear(GL_COLOR_BUFFER_BIT);

		screenShader.use();
		glBindVertexArray(quadVAO);
		glBindTexture(GL_TEXTURE_2D, framebuffer);	// use the color attachment texture as the texture of the quad plane
		glDrawArrays(GL_TRIANGLES, 0, 6);




		GLCheckError();



		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

	}

	//

	glDeleteProgram(ourShader.ID);
	glfwTerminate();
	return 0;
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}