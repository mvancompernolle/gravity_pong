#include "BallObject.h"

#include <iostream>
#include <ctime>
#include <cmath>
#include <ctime>

BallObject::BallObject() : GameObject(), radius(20.0f) {

}

BallObject::BallObject( glm::vec2 pos, GLfloat radius, Texture sprite, glm::vec2 vel )
	: GameObject( pos, glm::vec2( radius * 2, radius * 2 ), sprite, glm::vec4( 1.0f ), vel ), radius( radius ) {
}

BallObject::~BallObject() {

}