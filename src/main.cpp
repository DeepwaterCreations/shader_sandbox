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
#include "rectangle.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
GLFWwindow* setupWindow(int x, int y, int width, int height, const char* title);
unsigned int loadTextures(const char* filepath);

int main(int argv, char* argc[]){
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	const int width = 2560;
	const int height = 1440;

	//Set up viewport
	GLFWwindow* window = setupWindow(0, 0, width, height, "Magic Portal");
	if(window == NULL){
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	//Load textures
	unsigned int texture0 = loadTextures("textures/bluegrad.png");
	unsigned int texture1 = loadTextures("textures/mead_notebook_overlay.png");

	//Make a rectangle to show textures on
	Rectangle rectangle;

	//Load shader program
	ShaderProg shaderProg("/lair/ColdThings/shader_sandbox/src/shaders/vertex.glsl", "/lair/ColdThings/shader_sandbox/src/shaders/fragment.glsl");
	shaderProg.use();
	shaderProg.setInt("texture0", 0);
	shaderProg.setInt("texture1", 1);

	//Vectors and Matrices in GLM:
	//Translation:
	glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);	
	glm::mat4 trans = glm::mat4(1.0f); //<- Initializes the diagonal so it's an identity matrix
	trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));
	vec = trans * vec;
	std::cout << vec.x << vec.y << vec.z << std::endl;
	//Rotation:
	glm::mat4 rotat = glm::mat4(1.0f);
	rotat = glm::rotate(rotat, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
	rotat = glm::scale(rotat, glm::vec3(0.5, 0.5, 0.5));

	//Render Loop
	while(!glfwWindowShouldClose(window)){
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shaderProg.use();

		//Rotation:
		glm::mat4 rotator = glm::mat4(1.0f);
		rotator = glm::translate(rotator, glm::vec3(0.5f, -0.5f, 0.0f));
		rotator = glm::rotate(rotator, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
		//Remember that the matrices are applied in reverse order, so this rotates before
		//it translates.
		unsigned int transformLoc = glGetUniformLocation(shaderProg.ID, "transform");
		glUniformMatrix4fv(transformLoc, //The uniform's location
				1, //Number of matrices
				GL_FALSE, //Whether we should transpose the matrix
				glm::value_ptr(rotator)); //Actual data, converted from GLM to OpenGL format via value_ptr.

		/* float timeValue = glfwGetTime(); */
		/* float b = sin(timeValue)/2.0f + 0.5f; */
		/* shaderProg.setFloat("someColor", 1.0f); */
		/* int vertexColorLocation = glGetUniformLocation(shaderProgram, "someColor"); */
		/* glUniform4f(vertexColorLocation, 0.0f, b, 0.0f, 1.0f); */

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glBindVertexArray(rectangle.VAO);
		/* glDrawArrays(GL_TRIANGLES, 0, 3); //Primitive type, starting index, number of vertices */
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); //Primitive type, number of elements, index type, offset
		glBindVertexArray(0);

		glfwPollEvents();
		glfwSwapBuffers(window);
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
