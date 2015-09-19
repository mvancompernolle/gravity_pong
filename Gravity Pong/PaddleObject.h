#pragma once

#ifndef PADDLE_OBJECT_H
#define PADDLE_OBJECT_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

#include "GameObject.h"
#include "Texture.h"
#include "SpriteRenderer.h"
#include "ResourceManager.h"

enum PaddleDirection {
	PADDLE_UP,
	PADDLE_DOWN
};

class PaddleObject : public GameObject {
public:
	const GLfloat			ANIMATION_FRAME_TIME = 0.1f;
	GLfloat					stunnedTimer, speed, animationTime;
	std::vector<Texture>	animations;
	GLuint					animationIndex;

							PaddleObject( const glm::vec2 pos, const glm::vec2 size, const glm::vec4 color, const Texture sprite, const GLfloat speed = 1000.0f );
							~PaddleObject();
	void					move( const PaddleDirection dir );
	void					update( const GLfloat dt, const glm::vec2 heightRange );
	void					draw(SpriteRenderer& renderer);
	void					reset( const glm::vec2 pos );
};

#endif // PADDLE_OBJECT_H
