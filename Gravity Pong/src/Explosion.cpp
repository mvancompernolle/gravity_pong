#include "Explosion.h"


Explosion::Explosion( glm::vec2 pos, GLfloat radius, Texture sprite, GLfloat timeLeft )
	: BallObject( pos, radius, sprite ), MAX_RADIUS( radius ), timeLeft( timeLeft ), SHRINK_RATE( ( radius / 1.2f ) / timeLeft ) {

}

Explosion::~Explosion() {

}

void Explosion::update( const GLfloat dt ) {
	// have the explosion shirnk over time and become less bright
	if( timeLeft > 0.0f ) {
		timeLeft -= dt;
		radius -= SHRINK_RATE * dt;
		size = glm::vec2( radius * 2 );
		pos += SHRINK_RATE * dt;
		color = glm::vec4( glm::vec3( radius / MAX_RADIUS ), 1.0f );
	}
}
