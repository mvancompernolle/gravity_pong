#pragma once

// Reference: http://www.learnopengl.com/#!In-Practice/2D-Game/Setting-up

#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>

class Texture {
public:
				Texture();
				~Texture();
	void		generate(GLuint width, GLuint height, unsigned char* data);
	void		bind() const;

	GLuint		ID;
	GLuint		width, height;
	GLuint		internalFormat;
	GLuint		imageFormat;
	GLuint		wrapS, wrapT, filterMin, filterMax;
};

#endif //TEXTURE_H