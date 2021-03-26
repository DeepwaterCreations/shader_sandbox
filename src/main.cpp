#include <iostream>
#include <cmath>

#include <epoxy/gl.h>
#include <epoxy/glx.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"

#include "shaderprog.h"
#include "cube.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
GLFWwindow* setupWindow(int x, int y, int width, int height, const char* title);
unsigned int loadTextures(const char* filepath);

int main(int argv, char* argc[]){
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	const int WIDTH = 2560;
	const int HEIGHT = 1440;

	//Set up viewport
	GLFWwindow* window = setupWindow(0, 0, WIDTH, HEIGHT, "Magic Portal");
	if(window == NULL){
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	//Load textures
	unsigned int texture0 = loadTextures("textures/bluegrad.png");
	unsigned int texture1 = loadTextures("textures/mead_notebook_overlay.png");

	//Make a whole bunch of cubes
	glm::vec3 cubePositions[] ={
		glm::vec3( 0.0f,  0.0f,  0.0f),
		glm::vec3( 2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3( 2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3( 1.3f, -2.0f, -2.5f),
		glm::vec3( 1.5f,  2.0f, -2.5f),
		glm::vec3( 1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};
	Cube cube;

	//Load shader program
	ShaderProg shaderProg("/lair/ColdThings/shader_sandbox/src/shaders/vertex.glsl", "/lair/ColdThings/shader_sandbox/src/shaders/fragment.glsl");
	shaderProg.use();
	shaderProg.setInt("texture0", 0);
	shaderProg.setInt("texture1", 1);

	//Use depth testing
	glEnable(GL_DEPTH_TEST);

	//Render Loop
	while(!glfwWindowShouldClose(window)){
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderProg.use();

		//Model matrix: Object space => World space
		//Defined below at draw step
		//View matrix: World space => Camera space
		//"To move a camera backwards, is the same as moving the entire scene forward."
		glm::mat4 viewMatrix = glm::mat4(1.0f);
		viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0f, 0.0f, -3.0f));
		unsigned int viewMatrixLoc = glGetUniformLocation(shaderProg.ID, "viewMatrix");
		glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

		//Projection matrix: Camera space => Clip space
		glm::mat4 projectionMatrix;
		projectionMatrix = glm::perspective(glm::radians(45.0f), (float)WIDTH/(float)HEIGHT, 0.1f, 100.0f); //FOV, aspect ratio, near clipping, far clipping
		unsigned int projectionMatrixLoc = glGetUniformLocation(shaderProg.ID, "projectionMatrix");
		glUniformMatrix4fv(projectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glBindVertexArray(cube.VAO);
		for(int i = 0; i < 10; i++){
			glm::mat4 modelMatrix = glm::mat4(1.0f);
			modelMatrix = glm::translate(modelMatrix, cubePositions[i]);
			float angle = 20.0f * i;
			modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			shaderProg.setMat4("modelMatrix", modelMatrix);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		/* glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); //Primitive type, number of elements, index type, offset */
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}



	glfwTerminate();
	return 0;
}

//Callback for when the window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
	glViewport(0,0,width,height);
}

void processInput(GLFWwindow* window){
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
		glfwSetWindowShouldClose(window, true);
	}
}

GLFWwindow* setupWindow(int x, int y, int width, int height, const char* title){
	GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
	glfwMakeContextCurrent(window);
	glViewport(x, y, width, height);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	return window;
}

unsigned int loadTextures(const char* filepath){
	//Load a texture using the stb_image library and put it in an OpenGL texture
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	//Set texture wrapping/filtering options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //MAG for magnification

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 0);
	if(data){
		glTexImage2D(GL_TEXTURE_2D, //Texture target
				0, //Mipmap level for if you want to do those manually. (instead of that, we generate them below.)
				GL_RGB, //Texture storage format
				width, //We got the width and height values from the image
				height,//when we loaded it above, so we're using those.
				0, //Always 0. Legacy thingy.
				GL_RGB, //Source image format
				GL_UNSIGNED_BYTE, //Source image data type
				data); //The actual image data
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data); //We're done with the loaded image file now.
	return texture;	
}
