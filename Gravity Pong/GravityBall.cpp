#include "GravityBall.h"
#include <algorithm>
#include <iostream>

GravityBall::GravityBall( const glm::vec2 pos, const GLfloat radius, const Texture sprite, GLfloat energyCost, GLfloat speed, GLfloat maxRadius, GLfloat growthRate )
	: BallObject( pos, radius, sprite ), speed( speed ), MIN_RADIUS( radius ), MAX_RADIUS( radius * 2 ), GROWTH_RATE( growthRate ), ENERGY_TO_MAX( energyCost ), selectedBy( NO_ONE ),
	isCollapsing( GL_FALSE ), isReversed( GL_FALSE ) {
}


GravityBall::~GravityBall() {
}

void GravityBall::growBall( const GLfloat dt, GLfloat& energy ) {
	GLfloat oldRadius = radius;
	radius = std::min( MAX_RADIUS, radius + GROWTH_RATE * dt );
	size = glm::vec2( radius * 2.0f, radius * 2.0f );
	GLfloat cost = ( ( radius - oldRadius ) / ( MAX_RADIUS - MIN_RADIUS ) )  * ENERGY_TO_MAX;
	if( energy >= cost ) {
		energy -= ( ( radius - oldRadius ) / ( MAX_RADIUS - MIN_RADIUS ) )  * ENERGY_TO_MAX;
	} else {
		radius = oldRadius;
	}
}

void GravityBall::update( const GLfloat dt, const glm::vec2 heightRange ) {
	// rotate ball
	rotation += ROTATION_SPEED * dt;

	if( !isCollapsing ) {
		pos += vel * dt;
		// check if outside window bounds, if so reverse velocity and correct pos
		if( pos.y <= heightRange.x ) {
			vel.y = -vel.y;
			pos.y = heightRange.x;
		} else if( pos.y + size.y >= heightRange.y ) {
			vel.y = -vel.y;
			pos.y = heightRange.y - size.y;
		}
	} else {
		if( radius > 0.0f ) {
			glm::vec2 mid = getCenter();
			radius -= GROWTH_RATE / 5.0f * dt;
			size = glm::vec2( radius * 2.0f, radius * 2.0f );
			pos = mid - size / 2.0f;
		}
	}
}

void GravityBall::pullObject( const GLfloat dt, GameObject& object ) const {
	GLfloat dist = glm::distance( pos + radius, object.pos + size / 2.0f );
	if( dist <= MAX_RADIUS * 15.0f ) {
		// get pull direction
		glm::vec2 dir = glm::normalize( pos - object.pos );
		glm::vec2 pull = dir * ( 1.0f - ( dist / ( MAX_RADIUS * 15.0f ) ) ) * ( radius * object.mass ) * dt;
		if ( !isReversed ) {
			object.vel += pull;
		} else {
			object.vel -= pull;
		}

	}
}

void GravityBall::setReversed( const GLboolean reverse ) {
	this->isReversed = reverse;
}
