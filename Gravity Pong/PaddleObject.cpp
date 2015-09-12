#include "PaddleObject.h"

#include <iostream>

PaddleObject::PaddleObject( const glm::vec2 pos, const glm::vec2 size, const glm::vec4 color, const Texture sprite, const GLfloat speed )
	: GameObject( pos, size, sprite, color ), stunnedTimer( 0.0f ), speed( speed ) {
}

PaddleObject::~PaddleObject() {

}

void PaddleObject::move( const PaddleDirection dir ) {
	if( stunnedTimer <= 0.0f ) {
		if( dir == PADDLE_DOWN ) {
			vel.y = speed;
		} else if( dir == PADDLE_UP ) {
			vel.y = -speed;
		}
	} 
}

void PaddleObject::update( const GLfloat dt, const glm::vec2 heightRange ) {
	pos.y += vel.y * dt;
	if( pos.y > heightRange.y - size.y ) {
		pos.y = heightRange.y - size.y;
	} else if( pos.y < heightRange.x ) {
		pos.y = heightRange.x;
	}
	
	stunnedTimer -= dt;
	vel.y = 0.0f;
}

void PaddleObject::reset( const glm::vec2 pos) {
	this->vel = glm::vec2( 0.0f );
	this->pos = pos;
	stunnedTimer = 0.0f;
}
