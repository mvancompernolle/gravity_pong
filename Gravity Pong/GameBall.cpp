#include "GameBall.h"

#include <iostream>
#include <ctime>
#include <cmath>
#include <ctime>

const double PI = 3.14159265358;


GameBall::GameBall(){
	srand( time( NULL ) );
}

GameBall::GameBall( const glm::vec2 pos, const GLfloat radius, const GLfloat speed, const Texture sprite )
	: BallObject( pos, radius, sprite ), speed( speed ), isLaunching( GL_FALSE ), launchDT( 0.0f ), dir( 0.0f ) {
	srand( time( NULL ) );
}

GameBall::~GameBall() {

}

void GameBall::update( const GLfloat dt, const glm::vec2 heightRange ) {
	pos += vel * dt;
	// check if outside window bounds, if so reverse velocity and correct pos
	if ( pos.y <= heightRange.x ) {
		vel.y = -vel.y;
		pos.y = heightRange.x;
	} else if( pos.y + size.y >= heightRange.y ) {
		vel.y = -vel.y;
		pos.y = heightRange.y - size.y;
	}

	if ( isLaunching ) {
		launchDT -= dt;
		color = glm::vec3( std::sin( launchDT * 10 ), 0.3f, 0.3f );
		if ( launchDT <= 0 ) {
			isLaunching = GL_FALSE;
			launchDT = 0.0f;
			vel = glm::vec2( speed * cos( dir ), speed * sin( dir ) );
			color = glm::vec3( 1.0f );
		}
	}
}

void GameBall::reset( glm::vec2 position, glm::vec2 velocity ) {
	this->pos = position;
	this->vel = velocity;
	isLaunching = GL_FALSE;
	launchDT = 0.0f;
	color = glm::vec3( 1.0f );
}

void GameBall::startLaunch( const PLAYER_SELECTED selected ) {
	isLaunching = GL_TRUE;
	launchDT = PRE_LAUNCH_TIME;
	// launch the ball towards selected player, or randomly
	int player = selected;
	if( selected == NO_ONE ) {
		player = rand() % 2;
	}
	if( player == 0 ) {
		dir = rand() % 90 + 135;
	} else {
		dir = rand() % 90 + 315;
	}

	dir = dir * PI / 180;
}
