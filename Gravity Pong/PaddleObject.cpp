#include "PaddleObject.h"

#include <iostream>

PaddleObject::PaddleObject( const glm::vec2 pos, const glm::vec2 size, const glm::vec4 color, const Texture sprite, const GLfloat speed )
	: GameObject( pos, size, sprite, color ), stunnedTimer( 0.0f ), speed( speed ), animationTime( ANIMATION_FRAME_TIME ), animationIndex( 0 ) {
	if (pos.x < 500) {
		animations.push_back(ResourceManager::loadTexture("tech_paddle_1.png", GL_TRUE, "tech_paddle_1"));
		animations.push_back(ResourceManager::loadTexture("tech_paddle_2.png", GL_TRUE, "tech_paddle_2"));
		animations.push_back(ResourceManager::loadTexture("tech_paddle_3.png", GL_TRUE, "tech_paddle_3"));
		animations.push_back(ResourceManager::loadTexture("tech_paddle_4.png", GL_TRUE, "tech_paddle_4"));
	}
	else {
		animations.push_back(ResourceManager::loadTexture("alien_paddle_1.png", GL_TRUE, "alien_paddle_1"));
		animations.push_back(ResourceManager::loadTexture("alien_paddle_2.png", GL_TRUE, "alien_paddle_2"));
		animations.push_back(ResourceManager::loadTexture("alien_paddle_3.png", GL_TRUE, "alien_paddle_3"));
		animations.push_back(ResourceManager::loadTexture("alien_paddle_4.png", GL_TRUE, "alien_paddle_4"));
		animations.push_back(ResourceManager::loadTexture("alien_paddle_5.png", GL_TRUE, "alien_paddle_5"));
		animations.push_back(ResourceManager::loadTexture("alien_paddle_6.png", GL_TRUE, "alien_paddle_6"));
		animations.push_back(ResourceManager::loadTexture("alien_paddle_7.png", GL_TRUE, "alien_paddle_7"));
	}
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

	// update animations
	animationTime -= dt;
	if (animationTime <= 0.0f) {
		animationTime = ANIMATION_FRAME_TIME;
		animationIndex++;
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
