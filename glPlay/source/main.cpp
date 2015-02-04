#define GLEW_STATIC
#include "GL/glew.h"

#include "GLFW/glfw3.h"
#include "SOIL/SOIL.h"

#define GLM_FORCE_PURE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Triangle.h"
#include "Quad.h"

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

struct Sprite
{
	GLuint vao;
	GLuint vbo;
	float* vertices;
	GLuint shaderProgram;
	glm::mat4 transform;
	glm::vec3 translation, scale;
	float rotX, rotY, rotZ;
	glm::vec2 position;

	GLint uniMVP;
	GLuint uniTranslate;

	glm::mat4 viewMatrix;
	glm::mat4 cameraMatrix;
	glm::mat4 projectionMatrix;
	glm::mat4 modelViewProjectionMatrix;

	GLuint texture;


	Sprite(GLuint a_shaderProgram)
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);



		vertices = new float[16];
		vertices[0] = 0;//top-left
		vertices[1] = 100;
		vertices[2] = 0;//texture coord
		vertices[3] = 0;
		vertices[4] = 100;//top-right
		vertices[5] = 100;
		vertices[6] = 1;//texture coord
		vertices[7] = 0;
		vertices[8] = 100;//bottom-right
		vertices[9] = 0;
		vertices[10] = 1;//texture coord
		vertices[11] = 1;
		vertices[12] = 0;//bottom-left
		vertices[13] = 0;
		vertices[14] = 0;//texture coord
		vertices[15] = 1;

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		shaderProgram = a_shaderProgram;
		glBindFragDataLocation(shaderProgram, 0, "outColor");

		glLinkProgram(shaderProgram);
		glUseProgram(shaderProgram);

		GLint positionAttrib = glGetAttribLocation(shaderProgram, "position");
		glEnableVertexAttribArray(positionAttrib);
		glVertexAttribPointer(positionAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)* 4, 0);
		glEnableVertexAttribArray(positionAttrib);

		GLint textureCoordAttrib = glGetAttribLocation(shaderProgram, "texCoord");
		glEnableVertexAttribArray(textureCoordAttrib);
		glVertexAttribPointer(textureCoordAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)* 4, (void*)(sizeof(GL_FLOAT)* 2));
		glEnableVertexAttribArray(textureCoordAttrib);

		//load texture
		LoadTexture();

		uniMVP = glGetUniformLocation(shaderProgram, "MVP");

		transform = glm::mat4(1);
		position = glm::vec2(SCREEN_WIDTH * .5f, SCREEN_HEIGHT * .5f);
		translation = glm::vec3(position, 0);


		scale = glm::vec3(1);
		rotX = rotY = rotZ = 0;

		viewMatrix = glm::lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		projectionMatrix = glm::ortho(0.0f, 1024.0f, 0.0f, 768.0f);
		UpdateTransform();
	}

	~Sprite()
	{
		delete[] vertices;
	}

	void Update()
	{
		if (glfwGetKey(window, GLFW_KEY_A))
		{
			//translation += glm::vec3(-.1, 0, 0);
			rotZ += .001;
			UpdateTransform();
		}
		if (glfwGetKey(window, GLFW_KEY_D))
		{
			//translation += glm::vec3(.1, 0, 0);
			rotZ -= .001;
			UpdateTransform();
		}
		if (glfwGetKey(window, GLFW_KEY_ESCAPE))
		{
			glfwDestroyWindow(window);
		}


	}

	void Draw()
	{
		/*rotZ = 45;
		UpdateTransform();*/

		glDrawArrays(GL_QUADS, 0, 4);
		glUniformMatrix4fv(uniMVP, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));

	}

	void UpdateTransform()
	{



		glm::mat4 translationMatrix(1.0f);
		translationMatrix = glm::translate(translationMatrix, translation);
		glm::mat4 scaleMatrix(1.0f);
		scaleMatrix = glm::scale(scaleMatrix, scale);
		glm::mat4 rotationMatrix(1.0f);
		rotationMatrix = glm::rotate(rotationMatrix, rotX, glm::vec3(1, 0, 0));
		rotationMatrix = glm::rotate(rotationMatrix, rotY, glm::vec3(0, 1, 0));
		rotationMatrix = glm::rotate(rotationMatrix, rotZ, glm::vec3(0, 0, 1));
		transform = translationMatrix * rotationMatrix * scaleMatrix;



		//transform = scaleMatrix * rotationMatrix * translationMatrix;
		//transform[0].x = position.x;
		//transform[1].y = position.y;
		//transform[2].z = 0;
		//transform *= glm::vec4(position, 0, 1);
		//viewMatrix = viewMatrix * transform;

		modelViewProjectionMatrix = projectionMatrix * viewMatrix * transform;
		//modelViewProjectionMatrix = transform * viewMatrix * projectionMatrix;
		glm::vec4 pos1 = modelViewProjectionMatrix * glm::vec4(vertices[0], vertices[1], 0, 1);
		glm::vec4 pos2 = modelViewProjectionMatrix * glm::vec4(vertices[2], vertices[3], 0, 1);
		glm::vec4 pos3 = modelViewProjectionMatrix * glm::vec4(vertices[4], vertices[5], 0, 1);

		glUniformMatrix4fv(uniMVP, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));
	}

	void LoadTexture()
	{
		glGenTextures(1, &texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		int width, height;
		unsigned char* image = SOIL_load_image(".\\images\\testTexture.png", &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
};







int main()
{
	Initialize();
	CreateShaderProgram();
	Triangle triangle(shaderProgram, window);
	//Sprite sprite(shaderProgram);

	Quad quad(window, 100, 100);
	quad.translation += glm::vec3(100, 100, 0);
	quad.UpdateTransform();

	Quad myOtherQuad(window, 150,150);

	do
	{
		glClearColor(0, 1, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		//quad.Update();
		//quad.Draw();
		myOtherQuad.translation = glm::vec3(250, 250, 0);
		myOtherQuad.UpdateTransform();
		myOtherQuad.Update();
		myOtherQuad.Draw();
		//myOtherQuad.translation = glm::vec3(550, 550, 0);
		//myOtherQuad.UpdateTransform();
		//myOtherQuad.Draw();

		/*sprite.Update();
		sprite.Draw();*/

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
	//shaderProgram = CreateProgram(".\\source\\VertexShader.glsl", ".\\source\\TexturedFragmentShader.glsl");
	shaderProgram = CreateProgram(".\\source\\VertexShader.glsl", ".\\source\\FlatFragmentShader.glsl");
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