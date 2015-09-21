#pragma once

#ifndef GAME_BALL_H
#define GAME_BALL_H

#include <GL/glew.h>

#include <glm/glm.hpp>

#include "Texture.h"
#include "SpriteRenderer.h"
#include "BallObject.h"
#include "player_selected.h"
#include <irrklang/irrKlang.h>

const int PRE_LAUNCH_TIME = 3.0f;

class GameBall : public BallObject {
public:
	GLfloat			speed, launchDT, dir;
	GLboolean		isLaunching;

					GameBall( glm::vec2 pos, GLfloat radius, float speed, Texture sprite );
					~GameBall();
	void			update( const GLfloat dt, const glm::vec2 heightRange, irrklang::ISoundEngine& soundEngine );
	void			reset( glm::vec2 position, glm::vec2 velocity );
	void			startLaunch( const PLAYER_SELECTED selected = NO_ONE );
};

#endif //GAME_BALL_H