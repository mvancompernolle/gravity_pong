#pragma once

#ifndef POST_PROCESSOR_H
#define POST_PROCESSOR_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Texture.h"
#include "SpriteRenderer.h"
#include "Shader.h"

class PostProcessor {
public:
	Shader		shader;
	Texture		texture;
	GLuint		width, height;
	GLboolean	confuse, chaos, shake;

				PostProcessor( Shader shader, GLuint width, GLuint height );
				~PostProcessor();
	void		beginRender();
	void		endRender();
	void		render( GLfloat time );
private:
	GLuint	MSFBO, FBO;
	GLuint	RBO;
	GLuint	VAO;

	void	initRenderData();
};

#endif //POST_PROCESSOR_H