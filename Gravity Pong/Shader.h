#pragma once

// Reference: http://www.learnopengl.com/#!In-Practice/2D-Game/Setting-up

#ifndef SHADER_H
#define SHADER_H

#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:
	Shader();
	~Shader();
	Shader& use();
	void    compile(const GLchar *vertexSource, const GLchar *fragmentSource, const GLchar *geometrySource = nullptr);

	void    setFloat(const GLchar *name, GLfloat value);
	void    setInteger(const GLchar *name, GLint value);
	void    setVector2f(const GLchar *name, GLfloat x, GLfloat y);
	void    setVector2f(const GLchar *name, const glm::vec2 &value);
	void    setVector3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z);
	void    setVector3f(const GLchar *name, const glm::vec3 &value);
	void    setVector4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
	void    setVector4f(const GLchar *name, const glm::vec4 &value);
	void    setMatrix4(const GLchar *name, const glm::mat4 &matrix);

	GLuint ID;
private:
	void checkCompileErrors(GLuint object, std::string type);
};

#endif //SHADER_H

