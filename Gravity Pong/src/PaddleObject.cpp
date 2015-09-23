#include "PaddleObject.h"

#include <iostream>

PaddleObject::PaddleObject( glm::vec2 pos, glm::vec2 size, glm::vec4 color, Texture sprite, GLfloat speed )
	: GameObject( pos, size, sprite, color ), stunnedTimer( 0.0f ), speed( speed ), animationTime( ANIMATION_FRAME_TIME ), animationIndex( 0 ) {

	// set animation textures for first or second player
	if (pos.x < 500) {
		animations.push_back(ResourceManager::loadTexture("resources/images/tech_paddle_1.png", GL_TRUE, "tech_paddle_1"));
		animations.push_back(ResourceManager::loadTexture("resources/images/tech_paddle_2.png", GL_TRUE, "tech_paddle_2"));
		animations.push_back(ResourceManager::loadTexture("resources/images/tech_paddle_3.png", GL_TRUE, "tech_paddle_3"));
		animations.push_back(ResourceManager::loadTexture("resources/images/tech_paddle_4.png", GL_TRUE, "tech_paddle_4"));
	}
	else {
		animations.push_back(ResourceManager::loadTexture("resources/images/alien_paddle_1.png", GL_TRUE, "alien_paddle_1"));
		animations.push_back(ResourceManager::loadTexture("resources/images/alien_paddle_2.png", GL_TRUE, "alien_paddle_2"));
		animations.push_back(ResourceManager::loadTexture("resources/images/alien_paddle_3.png", GL_TRUE, "alien_paddle_3"));
		animations.push_back(ResourceManager::loadTexture("resources/images/alien_paddle_4.png", GL_TRUE, "alien_paddle_4"));
		animations.push_back(ResourceManager::loadTexture("resources/images/alien_paddle_5.png", GL_TRUE, "alien_paddle_5"));
		animations.push_back(ResourceManager::loadTexture("resources/images/alien_paddle_6.png", GL_TRUE, "alien_paddle_6"));
		animations.push_back(ResourceManager::loadTexture("resources/images/alien_paddle_7.png", GL_TRUE, "alien_paddle_7"));
	}
}

PaddleObject::~PaddleObject() {

}

void PaddleObject::move( const PaddleDirection dir ) {
	// set velocity if not stunned
	if( stunnedTimer <= 0.0f ) {
		if( dir == PADDLE_DOWN ) {
			vel.y = speed;
		} else if( dir == PADDLE_UP ) {
			vel.y = -speed;
		}
	} 
}

void PaddleObject::update( const GLfloat dt, const glm::vec2 heightRange ) {
	// update paddle position
	pos.y += vel.y * dt;
	if( pos.y > heightRange.y - size.y ) {
		pos.y = heightRange.y - size.y;
	} else if( pos.y < heightRange.x ) {
		pos.y = heightRange.x;
	}
	
	if ( stunnedTimer > 0.0f ) {
		stunnedTimer -= dt;
	}
	vel.y = 0.0f;

	// update animations
	animationTime -= dt;
	if (animationTime <= 0.0f) {
		// randomly choose next texture in animation
		animationTime = ANIMATION_FRAME_TIME;
		animationIndex = rand() % animations.size();
	}
}

void PaddleObject::reset( const glm::vec2 pos) {
	this->vel = glm::vec2( 0.0f );
	this->pos = pos;
	stunnedTimer = 0.0f;
}

void PaddleObject::draw( SpriteRenderer& renderer ) {
	renderer.drawSprite(animations[animationIndex], pos, size, rotation, color);
}
