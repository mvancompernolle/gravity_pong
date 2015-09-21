#include "ResourceManager.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include <FreeImagePlus.h>

// Reference: http://www.learnopengl.com/#!In-Practice/2D-Game/Setting-up

// instantiate static variables
std::map<std::string, Texture> ResourceManager::textures;
std::map<std::string, Shader> ResourceManager::shaders;

Shader ResourceManager::loadShader(const GLchar* vShaderFile, const GLchar* fShaderFile, const GLchar* gShaderFile, std::string name) {
	if (shaders.count(name) == 0) {
		shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
	}
	return shaders[name];
}

Shader ResourceManager::getShader(std::string name) {
	if( shaders.count( name ) == 0 ) {
		std::cout << "shader " << name << " not found." << std::endl;
	}
	return shaders[name];
}

Texture ResourceManager::loadTexture(const GLchar* file, GLboolean alpha, std::string name) {
	if (textures.count(name) == 0) {
		textures[name] = loadTextureFromFile(file, alpha);
	}
	return textures[name];
}

Texture ResourceManager::getTexture(std::string name) {
	if( textures.count( name ) == 0 ) {
		std::cout << "texture " << name << " not found." << std::endl;
	}
	return textures[name];
}

void ResourceManager::clear() {
	for (auto iter : shaders)
		glDeleteProgram(iter.second.ID);
	for (auto iter : textures)
		glDeleteTextures(1, &iter.second.ID);
}

Shader ResourceManager::loadShaderFromFile(const GLchar* vShaderFile, const GLchar* fShaderFile, const GLchar* gShaderFile) {
	std::string vertexCode, fragmentCode, geometryCode;

	try {
		// open the files
		std::ifstream vertexShaderFile(vShaderFile);
		std::ifstream fragmentShaderFile(fShaderFile);
		std::stringstream vShaderStream, fShaderStream;

		// read file's buffer contents into streams
		vShaderStream << vertexShaderFile.rdbuf();
		fShaderStream << fragmentShaderFile.rdbuf();
		vertexShaderFile.close();
		fragmentShaderFile.close();

		// convert stream to string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();

		// if geometry path was passed in, load the geometry shader
		if (gShaderFile != nullptr) {
			std::ifstream geoShaderFile(gShaderFile);
			std::stringstream gShaderStream;
			gShaderStream << geoShaderFile.rdbuf();
			geoShaderFile.close();
			geometryCode = gShaderStream.str();
		}
	}
	catch (std::exception e) {
		std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
	}

	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar* fShaderCode = fragmentCode.c_str();
	const GLchar* gShaderCode = geometryCode.c_str();

	// create the shader object from source code
	Shader shader;
	shader.compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr);
	return shader;
}

Texture ResourceManager::loadTextureFromFile(const GLchar* file, GLboolean alpha) {
	// create a texture object
	Texture texture;
	if (alpha) {
		texture.internalFormat = GL_RGBA;
		texture.imageFormat = GL_BGRA;
	}

	// load the texture
	fipImage image;
	if (image.load(file)) {
		if (image.getImageType() == FIT_UNKNOWN) {
			std::cerr << "Unknown image type for texture: " << file << std::endl;
		}
		
		if (alpha) {
			image.convertTo32Bits();
		}
		image.flipVertical();

		texture.generate(image.getWidth(), image.getHeight(), image.accessPixels());
	}
	else {
		std::cout << "Failed to load texture " << file << std::endl;
	}
	return texture;
}