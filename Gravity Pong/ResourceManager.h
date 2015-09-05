#pragma once

#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <map>
#include <string>

#include <GL/glew.h>

#include "Texture.h"
#include "Shader.h"

class ResourceManager {
public:
	~ResourceManager();

	static Shader loadShader(const GLchar* vShaderFile, const GLchar* fShaderFile, const GLchar* gShaderFile, std::string name);
	static Shader getShader(std::string name);
	static Texture loadTexture(const GLchar* file, GLboolean alpha, std::string name);
	static Texture getTexture(std::string name);
	static void clear();


	static std::map<std::string, Shader> shaders;
	static std::map<std::string, Texture> textures;

private:
	ResourceManager();
	static Shader loadShaderFromFile(const GLchar* vShaderFile, const GLchar* fShaderFile, const GLchar* gShaderFile = nullptr);
	static Texture loadTextureFromFile(const GLchar* file, GLboolean alpha);
};

#endif //RESOURCE_MANAGER_H