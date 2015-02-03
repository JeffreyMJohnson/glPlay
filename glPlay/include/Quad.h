#define GLEW_STATIC
#include "GL/glew.h"

#include "GLFW/glfw3.h"

#define GLM_FORCE_PURE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "SOIL/SOIL.h"

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#define PI 3.14159265359
#define RADIAN_CONVERSION PI / 180

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

struct Quad
{
	GLFWwindow* window;
	GLuint vao;
	GLuint vbo;
	GLuint uvo;//buffer for UV coordinates
	float* vertices;
	float* UVData;
	const char* vertexShaderPath = ".\\source\\VertexShader.glsl";
	const char* fragmentShaderPath = ".\\source\\TexturedFragmentShader.glsl";
	//const char* fragmentShaderPath = ".\\source\\FlatFragmentShader.glsl";
	const char* texturePath = ".\\resources\\textures\\testTexture.png";
	GLuint textureID;
	GLuint tex_location;
	GLuint shaderProgram;
	glm::mat4 transform;
	glm::vec3 translation, scale;
	float rotX, rotY, rotZ;
	glm::vec2 position;
	float width, height;

	GLint uniMVP;
	GLuint uniTranslate;

	glm::mat4 viewMatrix;
	glm::mat4 cameraMatrix;
	glm::mat4 projectionMatrix;
	glm::mat4 modelViewProjectionMatrix;


	Quad(GLFWwindow* a_windowHandle, float a_width, float a_height)
	{

		window = a_windowHandle;
		width = a_width;
		height = a_height;

		LoadTexture();

		shaderProgram = CreateProgram(vertexShaderPath, fragmentShaderPath);
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);

		glGenBuffers(1, &uvo);

		float hHeight = height * .5;
		float hWidth = width * .5;

		vertices = new float[8];
		vertices[0] = -hWidth;//top-left
		vertices[1] = hHeight;
		vertices[2] = hWidth;//top-right
		vertices[3] = hHeight;
		vertices[4] = hWidth;//bottom-right
		vertices[5] = -hHeight;
		vertices[6] = -hWidth;//bottom-left
		vertices[7] = -hHeight;

		UVData = new float[8];
		UVData[0] = 0;//top-left
		UVData[1] = 1;
		UVData[2] = 1;//top-right
		UVData[3] = 1;
		UVData[4] = 1;//bottom-right
		UVData[5] = 0;
		UVData[6] = 0;//bottom-left
		UVData[7] = 0;
		

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 8, vertices, GL_STATIC_DRAW);

		glBindFragDataLocation(shaderProgram, 0, "outColor");

		glLinkProgram(shaderProgram);
		glUseProgram(shaderProgram);

		GLint positionAttrib = glGetAttribLocation(shaderProgram, "position");
		glEnableVertexAttribArray(positionAttrib);
		glVertexAttribPointer(positionAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(positionAttrib);

		glBindBuffer(GL_ARRAY_BUFFER, uvo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(UVData), UVData, GL_STATIC_DRAW);
		GLint texAttrib = glGetAttribLocation(shaderProgram, "texCoord");
		glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(float)* 2, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		uniMVP = glGetUniformLocation(shaderProgram, "MVP");

		transform = glm::mat4(1);
		position = glm::vec2(SCREEN_WIDTH * .5f, SCREEN_HEIGHT * .5f);
		translation = glm::vec3(position, 0);


		scale = glm::vec3(1);
		rotX = rotY = rotZ = 0;
		//rotZ = DegreeToRadians(45.0f);
		//rotZ = 45.0f;
		viewMatrix = glm::lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		//projectionMatrix = glm::perspective(
		//	45.0f,
		//	1024.0f / 768.0f,
		//	.1f,
		//	100.0f
		//	);

		//viewMatrix = glm::mat4(1.0f);

		projectionMatrix = glm::ortho(0.0f, 1024.0f, 0.0f, 768.0f);



		UpdateTransform();

	}

	~Quad()
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
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}

	void Draw()
	{
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
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

	void Translate(glm::vec3 a_translation)
	{
		translation = a_translation;
	}

	void Scale(glm::vec3 a_scale)
	{
		scale = a_scale;
	}

	float DegreeToRadians(float degrees)
	{
		return degrees * RADIAN_CONVERSION;
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

	void LoadTexture()
	{
		textureID = 0;

		glGenTextures(1, &textureID);
		glActiveTexture(GL_TEXTURE0);

		//int imageWidth, imageHeight;
		//unsigned char* image = SOIL_load_image(texturePath, &imageWidth, &imageHeight, 0, SOIL_LOAD_RGBA);
		//glBindTexture(GL_TEXTURE_2D, textureID);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		//SOIL_free_image_data(image);

		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		textureID = SOIL_load_OGL_texture(texturePath, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT);

		tex_location = glGetUniformLocation(shaderProgram, "TextureSample");
	}


};