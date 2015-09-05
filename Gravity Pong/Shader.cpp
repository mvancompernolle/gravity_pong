#include "Shader.h"

#include <iostream>

Shader::Shader() {
	ID = 0;
}

Shader::~Shader() {

}

Shader& Shader::use() {
	glUseProgram(ID);
	return *this;
}

void    Shader::compile(const GLchar *vertexSource, const GLchar *fragmentSource, const GLchar *geometrySource) {
	GLuint sVertex, sFragment, sGeometry;

	// vertex shader
	sVertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(sVertex, 1, &vertexSource, NULL);
	glCompileShader(sVertex);
	checkCompileErrors(sVertex, "VERTEX");

	// fragment shader
	sFragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(sFragment, 1, &fragmentSource, NULL);
	glCompileShader(sFragment);
	checkCompileErrors(sFragment, "FRAGMENT");

	// geometry shader if source code is given
	if (geometrySource != nullptr) {
		sGeometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(sGeometry, 1, &geometrySource, NULL);
		glCompileShader(sGeometry);
		checkCompileErrors(sGeometry, "GEOMETRY");
	}

	// create the shader program
	ID = glCreateProgram();
	glAttachShader(ID, sVertex);
	glAttachShader(ID, sFragment);
	if (geometrySource != nullptr) {
		glAttachShader(ID, sGeometry);
	}
	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");

	// delete shaders
	glDeleteShader(sVertex);
	glDeleteShader(sFragment);
	if (geometrySource != nullptr) {
		glDeleteShader(sGeometry);
	}
}

void    Shader::setFloat(const GLchar *name, GLfloat value) {
	glUniform1f(glGetUniformLocation(this->ID, name), value);
}

void    Shader::setInteger(const GLchar *name, GLint value) {
	glUniform1i(glGetUniformLocation(this->ID, name), value);
}

void    Shader::setVector2f(const GLchar *name, GLfloat x, GLfloat y) {
	glUniform2f(glGetUniformLocation(this->ID, name), x, y);
}

void    Shader::setVector2f(const GLchar *name, const glm::vec2 &value) {
	glUniform2f(glGetUniformLocation(this->ID, name), value.x, value.y);
}

void    Shader::setVector3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z) {
	glUniform3f(glGetUniformLocation(this->ID, name), x, y, z);
}

void    Shader::setVector3f(const GLchar *name, const glm::vec3 &value) {
	glUniform3f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z);
}

void	Shader::setVector4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
	glUniform4f(glGetUniformLocation(this->ID, name), x, y, z, w);
}
void    Shader::setVector4f(const GLchar *name, const glm::vec4 &value) {
	glUniform4f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z, value.w);
}

void    Shader::setMatrix4(const GLchar *name, const glm::mat4 &matrix) {
	glUniformMatrix4fv(glGetUniformLocation(this->ID, name), 1, GL_FALSE, glm::value_ptr(matrix));
}

void	Shader::checkCompileErrors(GLuint object, std::string type) {
	GLint success;
	GLchar infoLog[1024];

	if (type != "PROGRAM") {
		glGetShaderiv(object, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(object, 1024, NULL, infoLog);
			std::cout << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n"
				<< infoLog << "\n -- --------------------------------------------------- -- "
				<< std::endl;
		}
	}
	else {
		glGetProgramiv(object, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(object, 1024, NULL, infoLog);
			std::cout << "| ERROR::Shader: Link-time error: Type: " << type << "\n"
				<< infoLog << "\n -- --------------------------------------------------- -- "
				<< std::endl;
		}
	}
}