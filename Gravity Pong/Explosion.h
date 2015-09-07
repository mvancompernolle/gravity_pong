#pragma once
#include "BallObject.h"

#ifndef EXPLOSION_H
#define EXPLOSION_H

class Explosion : public BallObject {
public:
	GLfloat			MAX_RADIUS, SHRINK_RATE;
	GLfloat			timeLeft;

					Explosion( glm::vec2 pos, GLfloat radius, Texture sprite, GLfloat timeLeft );
					~Explosion();
	void			update( const GLfloat dt );
};

#endif EXPLOSION_H