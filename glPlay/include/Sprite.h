//#define GLEW_STATIC
//#include "GL/glew.h"
//
//#include "GLFW/glfw3.h"
//
//#define GLM_FORCE_PURE
//#include "glm/glm.hpp"
//#include "glm/gtc/matrix_transform.hpp"
//#include "glm/gtc/type_ptr.hpp"

#include "SOIL/SOIL.h"

#include "Quad.h"


class Sprite
{
public:
	GLuint uvo;//buffer for UV coordinates
	GLuint shaderProgram;

	GLuint textureID;
	GLuint tex_location;

	float width, height;

	const char* vertexShaderPath = ".\\source\\VertexShader.glsl";
	const char* fragmentShaderPath = ".\\source\\TexturedFragmentShader.glsl";
	const char* texturePath = ".\\resources\\textures\\testTexture.png";

	Quad quad;

	float UVData[8]
	{
		0, 1,
		1, 1,
		1, 0,
		0, 0
	};

	Sprite()
	{
		LoadTexture();
	}

private:
	void LoadTexture()
	{
		textureID = 0;

		//glGenTextures(1, &textureID);
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

		glBindTexture(GL_TEXTURE_2D, textureID);
		tex_location = glGetUniformLocation(shaderProgram, "TextureSample");
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

};