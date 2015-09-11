#include "LeechAttack.h"
#include <iostream>

LeechAttack::LeechAttack( glm::vec2 pos, GLfloat radius, Texture sprite, glm::vec2 dir, GLfloat leechAmount, GLfloat duration ) 
	: BallObject( pos, radius, sprite, dir ), amountLeeched( 0.0f ), MAX_LEECH_AMOUNT( leechAmount ), LAUNCH_DIRECTION( dir ), timeLeft( duration ), attachedTo( nullptr ), isAlive( GL_TRUE ),
	STARTING_RADIUS( radius ), MAX_RADIUS ( radius * 2.0f ){
	
}

LeechAttack::~LeechAttack(){

}

void LeechAttack::update( const GLfloat dt ){
	if ( attachedTo == nullptr ) {
		pos += vel * dt;
	} else {
		timeLeft -= dt;
		if ( timeLeft <= 0.0f ) {
			detachLeech();
		} else {
			pos = attachedTo->pos - offset;
		}
	}
}

void LeechAttack::attachLeech( const GameObject* object ){
	std::cout << "attach" << std::endl;
	attachedTo = object;
	offset = object->pos - pos;
	vel = glm::vec2( 0.0f );
}

void LeechAttack::detachLeech(){
	attachedTo = nullptr;
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

	if ( attachedTo != nullptr ) {
		offset = attachedTo->pos - pos;
	}

	return leftOverEnergy;
}
