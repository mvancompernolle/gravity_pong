#pragma once

#ifndef PADDLE_OBJECT_H
#define PADDLE_OBJECT_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "GameObject.h"
#include "Texture.h"
#include "SpriteRenderer.h"

enum PaddleDirection {
	PADDLE_UP,
	PADDLE_DOWN
};

class PaddleObject : public GameObject {
public:
	GLfloat		stunnedTimer, speed;

				PaddleObject( const glm::vec2 pos, const glm::vec2 size, const glm::vec3 color, const Texture sprite, const GLfloat speed = 1000.0f );
				~PaddleObject();
	void		move( const PaddleDirection dir );
	void		update( const GLfloat dt, const glm::vec2 heightRange );
	void		reset( const glm::vec2 pos );
};

#endif // PADDLE_OBJECT_H
