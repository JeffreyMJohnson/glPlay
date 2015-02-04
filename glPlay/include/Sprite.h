#define GLEW_STATIC
#include "GL/glew.h"

#include "GLFW/glfw3.h"

#define GLM_FORCE_PURE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

//#include "SOIL/SOIL.h"

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#define PI 3.14159265359
#define RADIAN_CONVERSION PI / 180

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

struct Sprite
{
	GLFWwindow* window;
	GLuint vao;//vertex array object
	GLuint vbo;//vertex buffer object
	GLuint uvo;//buffer for UV coordinates

	float* vertices;
	GLuint textureID;
	GLuint uniTextureSample;
	GLuint shaderProgram;
	glm::mat4 transform;
	glm::vec3 translation, scale;
	float rotZ;
	//glm::vec2 position;
	float width, height;

	GLint uniMVP;

	glm::mat4 viewMatrix;
	glm::mat4 cameraMatrix;
	glm::mat4 projectionMatrix;
	glm::mat4 modelViewProjectionMatrix;


	Sprite()
	{

	}

	~Sprite()
	{

		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &uvo);
		glDeleteVertexArrays(1, &vao);
		glDeleteProgram(shaderProgram);
		delete[] vertices;
	}



	void Initialize(float a_width, float a_height, GLuint a_shaderProgram, char* texturePath)
	{
		shaderProgram = a_shaderProgram;
		glLinkProgram(shaderProgram);
		glUseProgram(shaderProgram);

		vertices = new float[16];
		vertices[0] = -.5;//top-left
		vertices[1] = .5;
		vertices[2] = 0;
		vertices[3] = 0;

		vertices[4] = .5;//top-right
		vertices[5] = .5;
		vertices[6] = 1;
		vertices[7] = 0;

		vertices[8] = .5;//bottom-right
		vertices[9] = -.5;
		vertices[10] = 1;
		vertices[11] = 1;

		vertices[12] = -.5;//bottom-left
		vertices[13] = -.5;
		vertices[14] = 0;
		vertices[15] = 1;

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, vertices, GL_STATIC_DRAW);

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		GLint positionAttrib = glGetAttribLocation(shaderProgram, "position");
		glEnableVertexAttribArray(positionAttrib);
		glVertexAttribPointer(positionAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(float)* 4, 0);

		GLint texCoordAttrib = glGetAttribLocation(shaderProgram, "texCoord");
		glEnableVertexAttribArray(texCoordAttrib);
		glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(float)* 4, (void*)(sizeof(float)* 2));


		uniMVP = glGetUniformLocation(shaderProgram, "MVP");

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		int imageWidth, imageHeight;
		unsigned char* image = SOIL_load_image(texturePath, &imageWidth, &imageHeight, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
		uniTextureSample = glGetUniformLocation(shaderProgram, "TextureSample");
		glUniform1i(uniTextureSample, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		transform = glm::mat4(1);
		translation = glm::vec3(0,0, 0);

		scale = glm::vec3(a_width, a_height, 1);
		rotZ = 0;

		viewMatrix = glm::lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		projectionMatrix = glm::ortho(0.0f, 1024.0f, 0.0f, 768.0f);



		UpdateTransform();
	}

	void Update()
	{

	}

	void SetUV(float minX, float minY, float maxX, float maxY)
	{

		//top-left
		vertices[2] = minX;
		vertices[3] = minY;
		//top-right
		vertices[6] = maxX;
		vertices[7] = minY;
		//bottom-right
		vertices[10] = maxX;
		vertices[11] = maxY;
		//bottom-left
		vertices[14] = minX;
		vertices[15] = maxY;
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 16, vertices, GL_STATIC_DRAW);

	}

	void Draw()
	{
		glDrawArrays(GL_QUADS, 0, 4);

		glUniformMatrix4fv(uniMVP, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));

	}

	void UpdateTransform()
	{

		glm::mat4 translationMatrix(1.0f);
		translationMatrix = glm::translate(glm::mat4(), translation);
		glm::mat4 scaleMatrix(1.0f);
		scaleMatrix = glm::scale(scaleMatrix, scale);
		glm::mat4 rotationMatrix(1.0f);
		rotationMatrix = glm::rotate(rotationMatrix, rotZ, glm::vec3(0, 0, 1));
		transform = translationMatrix * rotationMatrix * scaleMatrix;

		modelViewProjectionMatrix = projectionMatrix * viewMatrix * transform;

		glUniformMatrix4fv(uniMVP, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));
	}

	float DegreeToRadians(float degrees)
	{
		return degrees * RADIAN_CONVERSION;
	}

};