#pragma once

#ifndef MISSILE_H
#define MISSILE_H

#include "GameObject.h"
#include <irrklang/irrKlang.h>

class Missile :
	public GameObject {
public:
	const GLfloat	MAX_SPEED;
	const GLfloat	TURN_RATE;
	const GLfloat	ACCELERATION;
	GLfloat			desiredRotation;
	GameObject		*defaultTarget, *target;
	GLfloat			minWidth, maxWidth, minHeight, maxHeight;
	glm::vec2		targetOffset;
	irrklang::ISound* sound;

					Missile( glm::vec2 pos, glm::vec2 size, Texture sprite, GameObject* target, GLfloat goalDirection, irrklang::ISoundEngine& soundEngine, GLfloat rotation = 0.0f, GLfloat maxSpeed = 750.0f, GLfloat turnRate = 360.0f );
					~Missile();
	void			update( const GLfloat dt );
	void			setBoundaries( const GLfloat minW, const GLfloat maxW, const GLfloat minH, const GLfloat maxH );
};

#endif MISSILE_H