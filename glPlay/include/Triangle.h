#define GLEW_STATIC
#include "GL/glew.h"

#include "GLFW/glfw3.h"

#define GLM_FORCE_PURE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define PI 3.14159265359
#define RADIAN_CONVERSION PI / 180

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

struct Triangle
{
	GLFWwindow* window;
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

	Triangle(GLuint a_shaderProgram, GLFWwindow* a_windowHandle)
	{
		window = a_windowHandle;

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);


		//vertices = new float[6];
		//vertices[0] = 0;
		//vertices[1] = .5;
		//vertices[2] = .5;
		//vertices[3] = -.5;
		//vertices[4] = -.5;
		//vertices[5] = -.5;

		//vertices = new float[6];
		//vertices[0] = 512;
		//vertices[1] = 576;
		//vertices[2] = 768;
		//vertices[3] = 192;
		//vertices[4] = 256;
		//vertices[5] = 192;

		vertices = new float[6];
		vertices[0] = 0;
		vertices[1] = 50;
		vertices[2] = -20;
		vertices[3] = 0;
		vertices[4] = 20;
		vertices[5] = 0;

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 6, vertices, GL_STATIC_DRAW);

		shaderProgram = a_shaderProgram;
		glBindFragDataLocation(shaderProgram, 0, "outColor");

		glLinkProgram(shaderProgram);
		glUseProgram(shaderProgram);

		GLint positionAttrib = glGetAttribLocation(shaderProgram, "position");
		glEnableVertexAttribArray(positionAttrib);
		glVertexAttribPointer(positionAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(positionAttrib);

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

	~Triangle()
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
		/*rotZ = 45;
		UpdateTransform();*/
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		//glUniformMatrix4fv(uniMVP, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));
		glBindBuffer(GL_ARRAY_BUFFER, 0);

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


};