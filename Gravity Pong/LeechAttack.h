#pragma once

#ifndef LEECH_ATTACK_H
#define LEECH_ATTACK_H

#include "GameObject.h"

#include <GL/glew.h>
#include <irrklang/irrKlang.h>

class LeechAttack : public GameObject {
public:
	GLfloat				STRETCH_AMOUNT = 0.2f;
	GLfloat				amountLeeched, timeLeft, desiredRotation, TURN_RATE;
	GLuint				MAX_LEECH_AMOUNT;
	glm::vec2			STARTING_SIZE, MAX_SIZE;
	glm::vec2			LAUNCH_DIRECTION;
	const GameObject*	target;
	glm::vec2			offset;
	GLboolean			isAlive, isAttached, isReturning, isGettingEnergy;
	irrklang::ISound*	suckingSound;


						LeechAttack( glm::vec2 pos, glm::vec2 size, Texture sprite, glm::vec2 dir, GameObject* target, GLfloat leechAmount = 300.0f, GLfloat duration = 8.0f );
						~LeechAttack();
	void				update( const GLfloat dt );
	void				attachLeech();
	void				detachLeech();
	GLfloat				addEnergy( const GLfloat energy, irrklang::ISoundEngine& soundEngine );
};

#endif