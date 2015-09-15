#include "Missile.h"
#include <cmath>
#include <iostream>

Missile::Missile( glm::vec2 pos, glm::vec2 size, Texture sprite, GameObject* target, GLfloat rotation, GLfloat maxSpeed, GLfloat turnRate )
	: GameObject( pos, size, sprite ), defaultTarget( target ), MAX_SPEED( maxSpeed ), TURN_RATE( turnRate ), ACCELERATION( maxSpeed * 5.0f ),
	desiredRotation( 0.0f ) {
	this->rotation = rotation;
	this->target = defaultTarget;
	this->mass = 100.0f;
}

Missile::~Missile() {

}

void Missile::update( const GLfloat dt ) {
	if( target != nullptr ) {
		target = defaultTarget;
	}

	GLboolean interceptMode = GL_TRUE;
	glm::vec2 predictedLocation;
	if( interceptMode ) {
		// update best way to intercept target
		GLfloat relativeSpeed = std::abs( glm::length( vel ) - glm::length( target->vel ) );
		GLfloat dist = glm::distance( getCenter(), target->getCenter() );
		GLfloat time = 0.0f;
		if( relativeSpeed != 0.0f ) {
			time = dist / relativeSpeed;
		}
		predictedLocation = target->getCenter() + target->vel * time;

		// use x buffer to prevent colliding with players
		GLfloat xBuffer = ( maxWidth - minWidth ) * 0.05f;
		if( predictedLocation.x < minWidth + xBuffer || predictedLocation.x > maxWidth - xBuffer || predictedLocation.y < minHeight || predictedLocation.y > maxHeight ) {
			predictedLocation = target->getCenter();
			//std::cout << "follow" << std::endl;
		} else {
			//std::cout << "intercept" << std::endl;
		}

	} else {
		predictedLocation = target->getCenter();
	}

	// determine desired direction to intercept
	glm::vec2 posDiff = predictedLocation - getCenter();
	// rotation 0 to 360 going clockwise
	desiredRotation = glm::degrees( std::atan2( posDiff.y, posDiff.x ) );
	if( desiredRotation < 0 ) {
		desiredRotation += 360.0f;
	}

	GLboolean isTurning = GL_TRUE;
	if( std::abs( desiredRotation - rotation ) < TURN_RATE * dt ) {
		rotation = desiredRotation;
		isTurning = GL_FALSE;
		//std::cout << "not turning\n" << std::endl;
	} else {
		if( ( desiredRotation > rotation && desiredRotation - rotation <= 180 ) || ( desiredRotation < rotation && rotation - desiredRotation >= 180 ) ) {
			rotation += TURN_RATE * dt;
		} else {
			rotation -= TURN_RATE * dt;
		}

		// keep rotation is correct range
		if( rotation < 0 ) {
			rotation += 360.0f;
		} else if( rotation > 360.0f ) {
			rotation -= 360.0f;
		}
	}

	// update missile position
	GLfloat velChange = 0.0f;

	if( !isTurning ) {
		velChange = ACCELERATION * dt;
		vel.x += velChange * std::cos( glm::radians( -rotation ) );
		vel.y += velChange * -std::sin( glm::radians( -rotation ) );
	} else if( glm::length( vel ) > MAX_SPEED / 4.0f ) {
		GLfloat speed = glm::length( vel ) - (ACCELERATION * dt) / 5.0f;
		vel = glm::normalize( vel ) * speed;
	}

	if( glm::length( vel ) > MAX_SPEED ) {
		vel = glm::normalize( vel ) * MAX_SPEED;
	}
	pos += vel * dt;
}

void Missile::setBoundaries( const GLfloat minW, const GLfloat maxW, const GLfloat minH, const GLfloat maxH ) {
	minWidth = minW;
	maxWidth = maxW;
	minHeight = minH;
	maxHeight = maxH;
}

