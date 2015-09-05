#pragma once

#include <GL/glew.h>

#include "BallObject.h"
#include "player_selected.h"

#ifndef GRAVITY_BALL_H
#define GRAVITY_BALL_H

class GravityBall : public BallObject {
public:
	GLfloat			speed;
	GLfloat			ROTATION_SPEED = 180.0f;
	GLfloat			MIN_RADIUS, MAX_RADIUS;
	GLfloat			GROWTH_RATE;
	GLfloat			ENERGY_TO_MAX;
	PLAYER_SELECTED selectedBy;
	GLboolean		isCollapsing;

					GravityBall( const glm::vec2 pos, const GLfloat radius, const Texture sprite, GLfloat energyCost = 150, GLfloat speed = 500.0f, GLfloat maxRadius = 50.0f, GLfloat growthRate = 10.0f );
					~GravityBall();
	void			growBall( const GLfloat dt, GLfloat& energy );
	void			update( const GLfloat dt, const glm::vec2 heightRange );
	void			pullObject( const GLfloat dt, GameObject& object ) const;
};

#endif //GRAVITY_BALL_H

