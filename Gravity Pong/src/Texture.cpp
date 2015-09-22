#include "Texture.h"

// Reference: http://www.learnopengl.com/#!In-Practice/2D-Game/Setting-up

Texture::Texture() : width(0), height(0), internalFormat(GL_RGB), imageFormat(GL_BGR), wrapS(GL_REPEAT), wrapT(GL_REPEAT), filterMin(GL_LINEAR), filterMax(GL_LINEAR) {
	glGenTextures(1, &ID);
}

Texture::~Texture() {

}

void Texture::generate(GLuint width, GLuint height, unsigned char* data) {
	this->width = width;
	this->height = height;

	// create the texture
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, imageFormat, GL_UNSIGNED_BYTE, data);

	// set wrap and filter modes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMin);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMax);

	// unbind the texture
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind() const {
	glBindTexture(GL_TEXTURE_2D, ID);
}