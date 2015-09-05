#include "PaddleObject.h"

#include <iostream>

PaddleObject::PaddleObject( const glm::vec2 pos, const glm::vec2 size, const glm::vec3 color, const Texture sprite, const glm::vec2 vel )
	: GameObject( pos, size, sprite, color, vel ) {

}

PaddleObject::~PaddleObject() {

}

void PaddleObject::move( const PaddleDirection dir, const GLfloat dt, const glm::vec2 heightRange ) {
	if( dir == PADDLE_DOWN ) {
		pos.y += vel.y * dt;
		if( pos.y > heightRange.y - size.y ) {
			pos.y = heightRange.y - size.y;
		}
	} else if( dir == PADDLE_UP ) {
		pos.y -= vel.y * dt;
		if( pos.y < heightRange.x ) {
			pos.y = heightRange.x;
		}
	}
}

void PaddleObject::reset( const glm::vec2 pos) {
	this->vel = vel;
	this->pos = pos;
}
