#include "GameBall.h"

#include <iostream>
#include <ctime>
#include <cmath>
#include <ctime>

const double PI = 3.14159265358;

GameBall::GameBall( glm::vec2 pos, GLfloat radius, GLfloat speed, Texture sprite )
	: BallObject( pos, radius, sprite ), speed( speed ), isLaunching( GL_FALSE ), launchDT( 0.0f ), dir( 0.0f ) {
	srand( time( NULL ) );
	this->mass = 50.0f;
}

GameBall::~GameBall() {

}

void GameBall::update( const GLfloat dt, const glm::vec2 heightRange, irrklang::ISoundEngine& soundEngine ) {
	pos += vel * dt;
	// check if outside window bounds, if so reverse velocity and correct pos
	if ( pos.y <= heightRange.x ) {
		vel.y = -vel.y;
		pos.y = heightRange.x;

		// play wall bounce sound
		irrklang::ISound* sound = soundEngine.play2D( "resources/sounds/wall_bounce.wav", GL_FALSE, GL_TRUE );
		sound->setVolume( 0.75f );
		sound->setIsPaused( GL_FALSE );

	} else if( pos.y + size.y >= heightRange.y ) {
		vel.y = -vel.y;
		pos.y = heightRange.y - size.y;

		// play wall bounce sound
		irrklang::ISound* sound = soundEngine.play2D( "resources/sounds/wall_bounce.wav", GL_FALSE, GL_TRUE );
		sound->setVolume( 0.75f );
		sound->setIsPaused( GL_FALSE );
	}

	if ( isLaunching ) {
		launchDT -= dt;
		color = glm::vec4( std::sin( launchDT * 10 ), 0.3f, 0.3f, 1.0f );
		if ( launchDT <= 0 ) {
			// launch the ball in the chosen random direction
			isLaunching = GL_FALSE;
			launchDT = 0.0f;
			vel = glm::vec2( speed * cos( dir ), speed * sin( dir ) );
			color = glm::vec4( 1.0f );

			// play ball launch sound
			soundEngine.play2D( "resources/sounds/ball_launch.wav", GL_FALSE );
		}
	}
}

void GameBall::reset( glm::vec2 position, glm::vec2 velocity ) {
	this->pos = position;
	this->vel = velocity;
	isLaunching = GL_FALSE;
	launchDT = 0.0f;
	color = glm::vec4( 1.0f );
}

void GameBall::startLaunch( const PLAYER_SELECTED selected ) {
	isLaunching = GL_TRUE;
	launchDT = PRE_LAUNCH_TIME;

	// launch the ball towards selected player, or randomly if no player selected
	int player = selected;
	if( selected == NO_ONE ) {
		player = rand() % 2;
	}
	if( player == 0 ) {
		dir = rand() % 90 + 135;
	} else {
		dir = rand() % 90 + 315;
	}

	dir = dir * PI / 180.0f;
}
