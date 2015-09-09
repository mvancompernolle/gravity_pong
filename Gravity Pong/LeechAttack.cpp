#include "LeechAttack.h"


LeechAttack::LeechAttack( glm::vec2 pos, GLfloat radius, Texture sprite, glm::vec2 dir, GLfloat leechAmount, GLfloat duration ) 
	: BallObject( pos, radius, sprite, dir ), amountLeeched( 0.0f ), MAX_LEECH_AMOUNT( leechAmount ), LAUNCH_DIRECTION( dir ), timeLeft( duration ), attachedTo( nullptr ){
	
}

LeechAttack::~LeechAttack(){

}

void LeechAttack::update( const GLfloat dt ){
	if ( attachedTo == nullptr ) {
		pos += vel * dt;
	} else {
		pos = attachedTo->pos + offset;
		pos.x += radius;
	}
}

void LeechAttack::attachLeech( const GameObject* object ){
	attachedTo = object;
	offset = object->pos - pos;
	vel = glm::vec2( 0.0f );
}

void LeechAttack::detachLeech(){
	attachedTo = nullptr;
	offset = glm::vec2( 0.0f );
	vel = -LAUNCH_DIRECTION;
}

void LeechAttack::addEnergy( const GLfloat energy ){
	amountLeeched += energy;
}
