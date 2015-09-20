#include "GravityBall.h"
#include <algorithm>
#include <iostream>

GravityBall::GravityBall( const glm::vec2 pos, const GLfloat radius, const Texture sprite, GLfloat energyCost, GLfloat speed, GLfloat maxRadius, GLfloat growthRate )
	: BallObject( pos, radius, sprite ), speed( speed ), MIN_RADIUS( radius ), MAX_RADIUS( radius * 2 ), GROWTH_RATE( growthRate ), ENERGY_TO_MAX( energyCost ), selectedBy( NO_ONE ),
	isCollapsing( GL_FALSE ), isReversed( GL_FALSE ) {
}

GravityBall::~GravityBall() {
	if ( sound != nullptr ) {
		std::cout << "sound stopped" << std::endl;
		sound->stop();
		sound = nullptr;
	}
}

void GravityBall::growBall( const GLfloat dt, GLfloat& energy ) {
	GLfloat oldRadius = radius;
	radius = std::min( MAX_RADIUS, radius + GROWTH_RATE * dt );
	size = glm::vec2( radius * 2.0f, radius * 2.0f );
	GLfloat cost = ( ( radius - oldRadius ) / ( MAX_RADIUS - MIN_RADIUS ) )  * ENERGY_TO_MAX;
	if ( energy >= cost ) {
		energy -= ( ( radius - oldRadius ) / ( MAX_RADIUS - MIN_RADIUS ) )  * ENERGY_TO_MAX;
	} else {
		radius = oldRadius;
	}
}

void GravityBall::update( const GLfloat dt, const glm::vec2 heightRange ) {
	// rotate ball
	rotation += ROTATION_SPEED * dt;

	if ( !isCollapsing ) {
		pos += vel * dt;
		// check if outside window bounds, if so reverse velocity and correct pos
		if ( pos.y <= heightRange.x ) {
			vel.y = -vel.y;
			pos.y = heightRange.x;
		} else if ( pos.y + size.y >= heightRange.y ) {
			vel.y = -vel.y;
			pos.y = heightRange.y - size.y;
		}
	} else {
		if ( radius > 0.0f ) {
			glm::vec2 mid = getCenter();
			radius -= GROWTH_RATE / 10.0f * dt;
			size = glm::vec2( radius * 2.0f, radius * 2.0f );
			pos = mid - size / 2.0f;
		}
	}
}

void GravityBall::pullObject( const GLfloat dt, GameObject& object, irrklang::ISoundEngine& soundEngine ) {
	GLfloat dist = glm::distance( pos + radius, object.pos + size / 2.0f );
	if ( dist <= MAX_RADIUS * 15.0f ) {
		// get pull direction
		glm::vec2 dir = glm::normalize( pos - object.pos );
		glm::vec2 pull = dir * ( 1.0f - ( dist / ( MAX_RADIUS * 15.0f ) ) ) * ( radius * object.mass ) * dt;
		if ( !isReversed ) {
			object.vel += pull;
		} else {
			object.vel -= pull;
		}

		// if pull sound not playing, start playing
		if ( ( sound != nullptr && sound->isFinished() ) || sound == nullptr ) {
			sound = soundEngine.play2D( "gravity_sound.wav", GL_FALSE, GL_TRUE );
			sound->setVolume( 0.4f );
			sound->setPlayPosition( sound->getPlayLength() * ( 7.0f / 8.0f ) );
			sound->setIsPaused( GL_FALSE );
		}

	}
}

void GravityBall::setReversed( const GLboolean reverse ) {
	this->isReversed = reverse;
}
