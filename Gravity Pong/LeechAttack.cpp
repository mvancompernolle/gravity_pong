#include "LeechAttack.h"
#include <iostream>

LeechAttack::LeechAttack( glm::vec2 pos, GLfloat radius, Texture sprite, glm::vec2 dir, GameObject* target, GLfloat leechAmount, GLfloat duration ) 
	: BallObject( pos, radius, sprite, dir ), amountLeeched( 0.0f ), MAX_LEECH_AMOUNT( leechAmount ), LAUNCH_DIRECTION( dir ), timeLeft( duration ), target( target ), isAlive( GL_TRUE ), isAttached( GL_FALSE ),
	STARTING_RADIUS( radius ), MAX_RADIUS ( radius * 2.0f ), desiredRotation( 0.0f ), TURN_RATE( 90.0f ), isReturning( GL_FALSE ){
	if ( LAUNCH_DIRECTION.x < 0 ) {
		desiredRotation = 180.0f;
		rotation = 180.0f;
	}
}

LeechAttack::~LeechAttack(){

}

void LeechAttack::update( const GLfloat dt ){
	if ( !isAttached ) {

		if ( !isReturning ) {
			// get distance to the player
			GLfloat dist = glm::distance( getCenter(), target->getCenter() );
			if ( dist <= glm::length( vel ) / 3.0f ) {
				// determine desired direction to intercept
				glm::vec2 posDiff = target->getCenter() - getCenter();
				// rotation 0 to 360 going clockwise
				desiredRotation = glm::degrees( std::atan2( posDiff.y, posDiff.x ) );
				if ( desiredRotation < 0 ) {
					desiredRotation += 360.0f;
				}

				if ( std::abs( desiredRotation - rotation ) < TURN_RATE * dt ) {
					rotation = desiredRotation;
					//std::cout << "not turning\n" << std::endl;
				} else {
					if ( ( desiredRotation > rotation && desiredRotation - rotation <= 180 ) || ( desiredRotation < rotation && rotation - desiredRotation >= 180 ) ) {
						rotation += TURN_RATE * dt;
					} else {
						rotation -= TURN_RATE * dt;
					}

					// keep rotation is correct range
					if ( rotation < 0 ) {
						rotation += 360.0f;
					} else if ( rotation > 360.0f ) {
						rotation -= 360.0f;
					}
				}

				GLfloat speed = glm::length( vel );
				vel.x = speed * std::cos( glm::radians( -rotation ) );
				vel.y = speed * -std::sin( glm::radians( -rotation ) );
			}
		}

		pos += vel * dt;
	} else {
		timeLeft -= dt;
		if ( timeLeft <= 0.0f ) {
			detachLeech();
		} else {
			pos = target->pos - offset;
		}
	}
}

void LeechAttack::attachLeech(){
	isAttached = GL_TRUE;
	offset = target->pos - pos;
	vel = glm::vec2( 0.0f );
}

void LeechAttack::detachLeech(){
	isAttached = GL_FALSE;
	isReturning = GL_TRUE;
	offset = glm::vec2( 0.0f );
	vel = -LAUNCH_DIRECTION;
}

GLfloat LeechAttack::addEnergy( const GLfloat energy ){
	GLfloat leftOverEnergy = 0.0f;
	amountLeeched += energy;
	if ( amountLeeched > MAX_LEECH_AMOUNT ) {
		leftOverEnergy = amountLeeched - MAX_LEECH_AMOUNT;
		amountLeeched = MAX_LEECH_AMOUNT;
		detachLeech();
	}

	// calculate radius based on energy
	radius = STARTING_RADIUS + ( amountLeeched / MAX_LEECH_AMOUNT ) * ( MAX_RADIUS - STARTING_RADIUS );
	pos = getCenter() - radius;
	size = glm::vec2( radius * 2.0f );

	if ( isAttached ) {
		offset = target->pos - pos;
	}

	return leftOverEnergy;
}
