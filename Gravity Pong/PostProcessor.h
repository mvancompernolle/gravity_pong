#pragma once

#ifndef POST_PROCESSOR_H
#define POST_PROCESSOR_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Texture.h"
#include "SpriteRenderer.h"
#include "Shader.h"
#include "GameObject.h"

class PostProcessor {
public:
	Shader				shader;
	Texture				texture;
	GLuint				width, height;

						PostProcessor( Shader shader, GLuint width, GLuint height );
						~PostProcessor();
	void				beginRender();
	void				endRender();
	void				render( GLfloat time );
	void				blindPlayer( const GameObject& player, GLfloat blindRange, glm::vec2 heightRange );
	void				clearEffects();
private:
	GLuint				MSFBO, FBO;
	GLuint				RBO;
	GLuint				VAO;
	GLboolean			blind;
	const GameObject*	blindedPlayer;

	void	initRenderData();
};

#endif //POST_PROCESSOR_H