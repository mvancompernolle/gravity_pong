#pragma once

#ifndef GRAVITY_BALL_H
#define GRAVITY_BALL_H

#include <GL/glew.h>

#include "BallObject.h"
#include "player_selected.h"
#include <irrklang/irrKlang.h>

class GravityBall : public BallObject {
public:
	GLfloat				speed;
	GLfloat				ROTATION_SPEED = 180.0f;
	GLfloat				MIN_RADIUS, MAX_RADIUS;
	GLfloat				GROWTH_RATE;
	GLfloat				ENERGY_TO_MAX;
	PLAYER_SELECTED		selectedBy;
	GLboolean			isCollapsing, isReversed;
	irrklang::ISound*	sound;

						GravityBall( const glm::vec2 pos, const GLfloat radius, const Texture sprite, GLfloat energyCost = 150, GLfloat speed = 500.0f, GLfloat maxRadius = 50.0f, GLfloat growthRate = 20.0f );
						~GravityBall();
	void				growBall( const GLfloat dt, GLfloat& energy );
	void				update( const GLfloat dt, const glm::vec2 heightRange );
	void				pullObject( const GLfloat dt, GameObject& object, irrklang::ISoundEngine& soundEngine );
	void				setReversed( const GLboolean reverse );
};

#endif //GRAVITY_BALL_H

