#ifndef SHADERPROG_H
#define SHADERPROG_H

#include <epoxy/gl.h>
#include <epoxy/glx.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

//A class to read shaders from file
class ShaderProg{

	public:
		unsigned int ID;

		ShaderProg(const char* vertexPath, const char* fragmentPath){
			// First: Read the shader code from the files
			std::string vertexCode;
			std::string fragmentCode;
			std::ifstream vShaderFile;
			std::ifstream fShaderFile;

			//Enable exceptions
			vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

			try{
				//Open the files, read them into streams,
				//and convert the streams into strings.
				vShaderFile.open(vertexPath);
				fShaderFile.open(fragmentPath);
				std::stringstream vShaderStream, fShaderStream;
				vShaderStream << vShaderFile.rdbuf();
				fShaderStream << fShaderFile.rdbuf();
				vShaderFile.close();
				fShaderFile.close();
				vertexCode = vShaderStream.str();
				fragmentCode = fShaderStream.str();
			} catch(std::ifstream::failure& e){
				std::cout << "ERROR: Reading shader files failed:\n" << e.what() << std::endl;
			}

			const char* vShaderCode = vertexCode.c_str();
			const char* fShaderCode = fragmentCode.c_str();

			// Compile shaders
			unsigned int vertex, fragment;
			int success = 1;
			char infoLog[512];

			vertex = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex, 1, &vShaderCode, NULL);
			glCompileShader(vertex);
			glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
			if(!success){
				glGetShaderInfoLog(ID, 512, NULL, infoLog);
				std::cout << "ERROR: Vertex shader compilation failed:\n" << infoLog << std::endl;
			}

			fragment = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment, 1, &fShaderCode, NULL);
			glCompileShader(fragment);
			glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
			if(!success){
				glGetShaderInfoLog(ID, 512, NULL, infoLog);
				std::cout << "ERROR: Fragment shader compilation failed:\n" << infoLog << std::endl;
			}

			// Link shaders
			ID = glCreateProgram();
			glAttachShader(ID, vertex);
			glAttachShader(ID, fragment);
			glLinkProgram(ID);
			glGetProgramiv(ID, GL_LINK_STATUS, &success);
			if(!success){
				glGetProgramInfoLog(ID, 512, NULL, infoLog);
				std::cout << "ERROR: Shader program linkage failed:\n" << infoLog << std::endl;
			}

			//Clean up the no-longer-needed shader data.
			glDeleteShader(vertex);
			glDeleteShader(fragment);

		}

		void use(){
			glUseProgram(ID);
		}

		void setBool(const std::string &name, bool value) const{
			glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
		}
		void setInt(const std::string &name, int value) const{
			glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
		}
		void setFloat(const std::string &name, float value) const{
			glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
		}
};



#endif
