#include <iostream>
#include <cmath>

#include <epoxy/gl.h>
#include <epoxy/glx.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"

#include "shaderprog.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main(int argv, char* argc[]){
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	

	//Make a window
	GLFWwindow* window = glfwCreateWindow(800, 600, "Magic Portal", NULL, NULL);
	if(window == NULL){
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);


	//Set up viewport
	glViewport(0,0,800,600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	
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
	unsigned char* data = stbi_load("textures/bluegrad.png", &width, &height, &nrChannels, 0);
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


	//A Vertex Array Object: Keeps track of some state for us so we can
	//	easily draw our rectangle more than once.
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO); //Bind it so we can put the forthcoming triangle in it.
				//As long as it's bound, it'll remember the VBO and EBO stuff we define.

	//A simple rectangle made of two triangles
	float vertices[] = {
		//positions	     //colors           //texture coordinates
		 0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f, //Top right
		 0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 	1.0f, 0.0f, 
		-0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f
	};
	unsigned int VBO; // A vertex buffer object id
	glGenBuffers(1, &VBO);
	//Send the data to our buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO); //Bind our buffer to GL_ARRAY_BUFFER.
	//Copy the vertex data to the buffer currently bound to GL_ARRAY_BUFFER:
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //GL_DYNAMIC_DRAW if we expect this to change a lot.
	//Specify the format of our vertex data
	//Position attribute
	glVertexAttribPointer(0, //Location of starting attribute
			3, //Size of a vertex attribute: 3 values for x, y, z
			GL_FLOAT, //Type of the data
		       	GL_FALSE, //Does the data need to be converted to floats?
		       	8 * sizeof(float), //Stride: The space between consecutive vertex attributes
		       	(void*)0); //Offset of where data begins in the buffer
	glEnableVertexAttribArray(0);
	//Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	//Texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
	glEnableVertexAttribArray(2);

	//But how do we break the rectangle up into triangles? Like this:
	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3
	};
	unsigned int EBO; //An Element Buffer Object tells us which of the VBO vertices to draw in what order.
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	// Here we can unbind our VAO and make + bind the next one if we have more objects.


	ShaderProg shaderProg("/lair/ColdThings/shader_sandbox/src/shaders/vertex.glsl", "/lair/ColdThings/shader_sandbox/src/shaders/fragment.glsl");

	//Render Loop
	while(!glfwWindowShouldClose(window)){
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shaderProg.use();

		/* float timeValue = glfwGetTime(); */
		/* float b = sin(timeValue)/2.0f + 0.5f; */
		/* shaderProg.setFloat("someColor", 1.0f); */
		/* int vertexColorLocation = glGetUniformLocation(shaderProgram, "someColor"); */
		/* glUniform4f(vertexColorLocation, 0.0f, b, 0.0f, 1.0f); */

		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(VAO);
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
