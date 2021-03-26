#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <epoxy/gl.h>
#include <epoxy/glx.h>

class Rectangle{
	public:
		unsigned int VAO;
		unsigned int VBO;
		unsigned int EBO;
		Rectangle(){
			//A Vertex Array Object: Keeps track of some state for us so we can
			//	easily draw our rectangle more than once.
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
			
			// A vertex buffer object id:
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
			//An Element Buffer Object tells us which of the VBO vertices to draw in what order.
			glGenBuffers(1, &EBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
			// Here we can unbind our VAO and make + bind the next one if we have more objects.

		}

		~Rectangle(){
			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);
			glDeleteBuffers(1, &EBO);
		}
	
};

#endif
