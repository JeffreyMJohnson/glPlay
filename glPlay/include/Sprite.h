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
	GLint textureLocation;

	float width, height;

	const char* vertexShaderPath = ".\\source\\VertexShader.glsl";
	const char* fragmentShaderPath = ".\\source\\TexturedFragmentShader.glsl";
	const char* texturePath = ".\\resources\\textures\\testTexture.png";

	Quad quad;

	Transform transform;
	glm::mat4 orthoProjection;
	glm::mat4 viewMatrix;
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 rotation;
	glm::vec2 scale;

	Sprite()
	{
		//create shaders
		shaderProgram = CreateProgram(vertexShaderPath, fragmentShaderPath);
		quad.shaderProgram = shaderProgram;

		glUseProgram(quad.shaderProgram);

		float UVData[]
		{
			0, 1,
			1, 0,
			0, 1,
			1, 1
		};


		//load UV data into buffer
		glBindVertexArray(quad.vao);
		glGenBuffers(1, &uvo);
		glBindBuffer(GL_ARRAY_BUFFER, uvo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(UVData), UVData, GL_STATIC_DRAW);

		GLint texAttrib = glGetAttribLocation(quad.shaderProgram, "texCoord");
		glEnableVertexAttribArray(texAttrib);
		glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(float)* 2, 0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		orthoProjection = glm::ortho(0, 1024, 0, 768);
		viewMatrix = glm::lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	}

	void Update(float a_deltaTime)
	{
		transform.viewTranslate = glm::translate(glm::mat4(), glm::vec3(1024 * .5f, 768 * .5f, 1));
		transform.modelScale = glm::scale(glm::mat4(), glm::vec3(100, 100, 1));
		transform.MVP = orthoProjection * viewMatrix * transform.viewTranslate * transform.modelScale;
		quad.modelViewProjectionMatrix = transform.MVP;

		Draw();
	}

	void Draw()
	{
		glEnable(GL_BLEND);
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, .5);
		glEnable(GL_ALPHA);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CW);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureLocation);
		glUniform1i(textureLocation, 0);
		quad.Draw();
	}

	void LoadTexture(const char* a_texture)
	{
		textureID = 0;
		glGenTextures(1, &textureID);
		glActiveTexture(GL_TEXTURE0);
		


		int imageWidth, imageHeight;
		unsigned char* image = SOIL_load_image(a_texture, &imageWidth, &imageHeight, 0, SOIL_LOAD_RGBA);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);

		//wrapping params
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//filtering params
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


		textureLocation = glGetUniformLocation(quad.shaderProgram, "TextureSample");

	}

private:
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