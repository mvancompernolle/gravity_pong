#pragma once

#ifndef LEECH_ATTACK_H
#define LEECH_ATTACK_H

#include "BallObject.h"

#include <GL/glew.h>

class LeechAttack : public BallObject {
public:
	const GLuint		numBalls = 3, timeLeft;

	GLfloat				amountLeeched;
	const GLuint		MAX_LEECH_AMOUNT;
	const glm::vec2		LAUNCH_DIRECTION;
	const GameObject*	attachedTo;
	glm::vec2			offset;

						LeechAttack( glm::vec2 pos, GLfloat radius, Texture sprite, glm::vec2 dir, GLfloat leechAmount = 300.0f, GLfloat duration = 3.0f );
						~LeechAttack();
	void				update( const GLfloat dt );
	void				attachLeech( const GameObject* object );
	void				detachLeech();
	void				addEnergy( const GLfloat energy );
};

#endif