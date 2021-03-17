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
	//	easily draw our rectangle more than once.
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO); //Bind it so we can put the forthcoming triangle in it.
				//As long as it's bound, it'll remember the VBO and EBO stuff we define.

	//A simple rectangle made of two triangles
	float vertices[] = {
		0.5f, 0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f
	};
	unsigned int VBO; // A vertex buffer object id
	glGenBuffers(1, &VBO);
	//Send the data to our buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO); //Bind our buffer to GL_ARRAY_BUFFER.
	//Copy the vertex data to the buffer currently bound to GL_ARRAY_BUFFER:
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //GL_DYNAMIC_DRAW if we expect this to change a lot.
	//Specify the format of our vertex data
	glVertexAttribPointer(0, //Location of starting attribute
			3, //Size of a vertex attribute: 3 values for x, y, z
			GL_FLOAT, //Type of the data
		       	GL_FALSE, //Does the data need to be converted to floats?
		       	3 * sizeof(float), //Stride: The space between consecutive vertex attributes
		       	(void*)0); //Offset of where data begins in the buffer
	glEnableVertexAttribArray(0);

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

	
	//Basic vertex shader that does nothing to the vertex.
	//We just need to assign a w value of 1.0 to the fourth vertex position.
	const char* vertexShaderSource = "#version 330 core\n"
		"layout(location = 0) in vec3 aPos;\n"
		"out vec4 vertexColor;\n"
		"void main(){\n"
		"	gl_Position = vec4(aPos, 1.0);\n"
		"	vertexColor = vec4(0.0, 0.0, 0.5, 1.0);\n"
		"}\0";
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	//Check if compilation was successful:
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if(!success){
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR: Vertex shader compilation failed:\n" << infoLog << std::endl;
	}


	//Basic fragment shader that outputs the same color no matter what.
	const char* fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"in vec4 vertexColor;\n"
		"void main(){\n"
		"	FragColor = vertexColor;\n"
		"}\0";
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	//Check if compilation was successful:
	//int success; (Already declared, but I'm leaving this here in case I move this elsewhere) 
	//char infoLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if(!success){
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR: Fragment shader compilation failed:\n" << infoLog << std::endl;
	}


	//Link the shaders into a shader program.
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	//Check if linking failed:
	//int success; (Already declared)
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if(!success){
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR: Shader program linkage failed:\n" << infoLog << std::endl;
	}
	//Clean up the no-longer-needed shader data.
	glDeleteShader(vertexShader);
	glDeleteShader(vertexShader);


	//Render Loop
	while(!glfwWindowShouldClose(window)){
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
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
