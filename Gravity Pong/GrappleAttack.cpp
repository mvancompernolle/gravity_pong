#include "GrappleAttack.h"
#include <iostream>

GrappleAttack::GrappleAttack( GameObject* creator, GameObject* target, glm::vec2 pos, GLfloat anchorRadius, glm::vec2 vel, GLuint width, GLfloat duration )
	: creator( creator ), target( target ), closeAnchorOffset( pos - creator->getCenter() ), farAnchorOffset( glm::vec2( 0.0f ) ), isAlive( GL_TRUE ),
	isAttached( GL_FALSE ), gameWidth( width ), timeLeft( duration ), PULL_STRENGTH( 2.0f ) {
	// initialize close and far anchors
	anchors[0] = BallObject( creator->getCenter() + closeAnchorOffset, anchorRadius, ResourceManager::getTexture( "anchor" ) );
	for( int i = 1; i < NUM_ANCHORS - 1; i++ ) {
		anchors[i] = anchors[0];
	}
	anchors[NUM_ANCHORS - 1] = BallObject( creator->getCenter() + glm::vec2( 2.0f * closeAnchorOffset.x, closeAnchorOffset.y ), anchorRadius, ResourceManager::getTexture( "anchor" ), vel );
	// initialize links
	glm::vec4 color = vel.x > 0 ? glm::vec4( 0.0f, 1.0f, 0.0f, 0.5f ) : glm::vec4( 0.0f, 0.3f, 0.7f, 0.5f );
	for( int i = 0; i < NUM_LINKS; ++i ) {
		links[i] = GameObject( glm::vec2( 0.0f ), glm::vec2( 1.0f, anchorRadius * 0.75f ) , ResourceManager::getTexture("link"), color );
	}
}

GrappleAttack::~GrappleAttack() {

}

void GrappleAttack::update( const GLfloat dt ) {
	if( isAlive ) {
		if( anchors[NUM_ANCHORS - 1].pos.x > gameWidth || anchors[NUM_ANCHORS - 1].pos.x < -anchors[NUM_ANCHORS - 1].size.x || timeLeft <= 0.0f ) {
			isAlive = GL_FALSE;
			return;
		}

		anchors[0].pos = creator->getCenter() + closeAnchorOffset;

		if( !isAttached ) {
			anchors[NUM_ANCHORS - 1].pos += anchors[NUM_ANCHORS - 1].vel * dt;

			GLfloat yDiff = anchors[0].getCenter().y - anchors[NUM_ANCHORS - 1].getCenter().y;
			anchors[NUM_ANCHORS - 1].pos.y += PULL_STRENGTH * yDiff * dt;
		} else {
			timeLeft -= dt;
			//anchors[NUM_ANCHORS - 1].pos = target->getCenter() + farAnchorOffset;

			// pull the target paddle toward the creator paddle vertically
			GLfloat yDiff = anchors[0].getCenter().y - anchors[NUM_ANCHORS - 1].getCenter().y;
			target->pos.y += PULL_STRENGTH * yDiff * dt;

			anchors[NUM_ANCHORS - 1].pos = target->getCenter() + farAnchorOffset;
		}
	}

	glm::vec2 diff = anchors[NUM_ANCHORS - 1].pos - anchors[0].pos;
	// update the positions of intermediate anchors
	for( int i = 1; i < NUM_ANCHORS - 1; ++i ) {
		glm::vec2 desiredPos = anchors[0].pos + (GLfloat) i / ( NUM_ANCHORS - 1 ) * diff;
		anchors[i].pos = desiredPos;
	}
	// update links
	GLfloat angle = glm::degrees(std::atan2( diff.y, diff.x ));
	angle = angle >= 0 ? angle : angle + 360;
	GLfloat length = glm::length( diff ) / ( NUM_ANCHORS - 1 );
	for( int i = 0; i < NUM_LINKS; ++i ) {
		//links[i].rotation = 
		links[i].pos = ( anchors[i].getCenter() + anchors[i + 1].getCenter() ) / 2.0f;
		links[i].size.x = length;
		links[i].pos -= links[i].size / 2.0f;
		links[i].rotation = angle;
	}
}

void GrappleAttack::draw( SpriteRenderer& renderer ) {
	// render links
	for( GameObject& link : links ) {
		link.draw( renderer );
	}

	// render anchors
	for( BallObject& anchor : anchors ) {
		anchor.draw( renderer );
	}
}

void GrappleAttack::attachToTarget() {
	isAttached = GL_TRUE;
	farAnchorOffset = anchors[NUM_ANCHORS - 1].pos - target->getCenter();
}