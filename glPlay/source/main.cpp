#define GLEW_STATIC
#include "GL/glew.h"

#include "GLFW/glfw3.h"
#include "SOIL/SOIL.h"

#define GLM_FORCE_PURE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Sprite.h"

#include <string>
#include <iostream>
#include <time.h>
#include <fstream>
#include <vector>



extern const int SCREEN_WIDTH = 1024;
extern const int SCREEN_HEIGHT = 768;

GLFWwindow* window;
GLuint shaderProgram;

void Initialize();
void Destroy();

GLuint CreateProgram(const char* a_vertex, const char* a_frag);
GLuint CreateShader(GLenum a_ShaderType, const char* a_ShaderFile);
void CreateShaderProgram();
void HandleUI(Sprite& s);



int main()
{
	Initialize();
	CreateShaderProgram();
	//Sprite sprite(shaderProgram);


	Sprite s;
	s.Initialize(250, 250, shaderProgram, ".\\resources\\textures\\testTexture.png");


	do
	{
		glClearColor(.5, .5, .5, 1);
		glClear(GL_COLOR_BUFFER_BIT);


		s.translation = glm::vec3(250, 250, 0);
		s.UpdateTransform();
		s.SetUV(0, 0, .25, .25);
		s.Update();
		s.Draw();
		s.translation = glm::vec3(550, 550, 0);
		s.UpdateTransform();
		s.SetUV(0, .25, .25, .5);
		s.Draw();

		s.translation = glm::vec3(550, 250, 0);
		s.UpdateTransform();
		s.Draw();

		s.translation = glm::vec3(250, 550, 0);
		s.UpdateTransform();
		s.Draw();

		HandleUI(s);

		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (!glfwWindowShouldClose(window));

	Destroy();

	return 0;
}

void Initialize()
{
	glfwInit();
	//create the window
	//windowed
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL Bitches!", nullptr, nullptr);
	//full screen
	//window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL Bitches!", glfwGetPrimaryMonitor(), nullptr);

	//make window active
	glfwMakeContextCurrent(window);

	//force GLEW to use a modern OpenGL method for checking if function is available
	glewExperimental = GL_TRUE;
	//initialize GLEW
	glewInit();
}

void Destroy()
{
	glfwTerminate();
}

void CreateShaderProgram()
{
	shaderProgram = CreateProgram(".\\source\\VertexShader.glsl", ".\\source\\TexturedFragmentShader.glsl");
	//shaderProgram = CreateProgram(".\\source\\VertexShader.glsl", ".\\source\\FlatFragmentShader.glsl");
}

GLuint CreateShader(GLenum a_ShaderType, const char* a_ShaderFile)
{
	std::string shaderCode;
	//open shader file
	std::ifstream shaderStream(a_ShaderFile);
	//if that worked ok, load file line by line
	if (shaderStream.is_open())
	{
		std::string line = "";
		while (std::getline(shaderStream, line))
		{
			shaderCode += "\n" + line;
		}
		shaderStream.close();
	}

	//convert to cstring
	char const* shaderSourcePointer = shaderCode.c_str();

	//create shader ID
	GLuint shader = glCreateShader(a_ShaderType);
	//load source code
	glShaderSource(shader, 1, &shaderSourcePointer, NULL);

	//compile shader
	glCompileShader(shader);

	//check for errors
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar* infoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, infoLog);

		const char* shaderType = NULL;
		switch (a_ShaderType)
		{
		case GL_VERTEX_SHADER:
			shaderType = "vertex";
			break;
		case GL_FRAGMENT_SHADER:
			shaderType = "fragment";
			break;
		}

		fprintf(stderr, "Compile failure in %s shader:\n%s\n", shaderType, infoLog);
		delete[] infoLog;
	}

	return shader;

}

GLuint CreateProgram(const char* a_vertex, const char* a_frag)
{

	std::vector<GLuint> shaderList;

	shaderList.push_back(CreateShader(GL_VERTEX_SHADER, a_vertex));
	shaderList.push_back(CreateShader(GL_FRAGMENT_SHADER, a_frag));

	//create shader program ID
	GLuint program = glCreateProgram();

	//attach shaders
	for (auto shader = shaderList.begin(); shader != shaderList.end(); shader++)
	{
		glAttachShader(program, *shader);
	}

	//link program
	glLinkProgram(program);

	//check for link errors and output them
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar* infoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, infoLog);

		fprintf(stderr, "Linker failure: %s\n", infoLog);
		delete[] infoLog;
	}

	for (auto shader = shaderList.begin(); shader != shaderList.end(); shader++)
	{
		glDetachShader(program, *shader);
		glDeleteShader(*shader);
	}
	return program;
}

void HandleUI(Sprite& s)
{
	if (glfwGetKey(window, GLFW_KEY_A))
	{
		//translation += glm::vec3(-.1, 0, 0);
		s.rotZ += .001;
		s.UpdateTransform();
	}
	if (glfwGetKey(window, GLFW_KEY_D))
	{
		//translation += glm::vec3(.1, 0, 0);
		s.rotZ -= .001;
		s.UpdateTransform();
	}
	if (glfwGetKey(window, GLFW_KEY_ESCAPE))
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}