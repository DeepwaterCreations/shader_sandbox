#include <iostream>

#include <epoxy/gl.h>
#include <epoxy/glx.h>
#include <GLFW/glfw3.h>

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

	//A Vertex Array Object: Keeps track of some state for us so we can
	//	easily draw our triangle more than once.
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO); //Bind it so we can put the forthcoming triangle in it.

	//A simple triangle
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.0f
	};
	unsigned int VBO; // A vertex buffer object id
	glGenBuffers(1, &VBO);
	//Send the data to our buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO); //Bind our buffer to GL_ARRAY_BUFFER.
	//Copy the vertex data to the buffer currently bound to GL_ARRAY_BUFFER:
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//Specify the format of our vertex data
	glVertexAttribPointer(0, //Location of starting attribute
			3, //Size of a vertex attribute: 3 values for x, y, z
			GL_FLOAT, //Type of the data
		       	GL_FALSE, //Does the data need to be converted to floats?
		       	3 * sizeof(float), //Stride: The space between consecutive vertex attributes
		       	(void*)0); //Offset of where data begins in the buffer
	glEnableVertexAttribArray(0);
	// Here we can unbind our VAO and make + bind the next one if we have more objects.
	//Render Loop
	while(!glfwWindowShouldClose(window)){
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

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
