#pragma once

// Reference: http://learnopengl.com/#!In-Practice/2D-Game/Postprocessing

#ifndef POST_PROCESSOR_H
#define POST_PROCESSOR_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "ResourceManager.h"
#include "Texture.h"
#include "SpriteRenderer.h"
#include "Shader.h"
#include "GameObject.h"

// Reference:: http://www.learnopengl.com/#!In-Practice/2D-Game/Postprocessing

class PostProcessor {
public:
	Texture				texture;
	GLuint				width, height;

						PostProcessor( GLuint width, GLuint height );
						~PostProcessor();
	void				beginRender();
	void				endRender();
	void				render( GLfloat time );
	void				blindPlayer( const GameObject& player, GLfloat blindRange, glm::vec2 heightRange );
	void				flipScreen( glm::vec2 widthRange, glm::vec2 heightRange );
	void				clearEffects();
private:
	GLuint				MSFBO, FBO;
	GLuint				RBO;
	GLuint				VAO;
	GLboolean			blind, flip;
	Shader				defaultShader, blindShader, flipShader;
	const GameObject*	blindedPlayer;

	void	initRenderData();
};

#endif //POST_PROCESSOR_H