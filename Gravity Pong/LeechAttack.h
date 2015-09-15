#pragma once

#ifndef LEECH_ATTACK_H
#define LEECH_ATTACK_H

#include "BallObject.h"
#include "GameObject.h"

#include <GL/glew.h>

class LeechAttack : public BallObject {
public:
	GLfloat				amountLeeched, timeLeft, desiredRotation;
	GLuint				MAX_LEECH_AMOUNT;
	GLfloat				STARTING_RADIUS, MAX_RADIUS, TURN_RATE;
	glm::vec2			LAUNCH_DIRECTION;
	const GameObject*	target;
	glm::vec2			offset;
	GLboolean			isAlive, isAttached, isReturning;


						LeechAttack( glm::vec2 pos, GLfloat radius, Texture sprite, glm::vec2 dir, GameObject* target, GLfloat leechAmount = 300.0f, GLfloat duration = 8.0f );
						~LeechAttack();
	void				update( const GLfloat dt );
	void				attachLeech();
	void				detachLeech();
	GLfloat				addEnergy( const GLfloat energy );
};

#endif