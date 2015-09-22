#include "GrappleAttack.h"
#include <iostream>

GrappleAttack::GrappleAttack( GameObject* creator, GameObject* target, glm::vec2 pos, GLfloat anchorRadius, glm::vec2 vel, GLuint width, GLfloat duration )
	: creator( creator ), target( target ), closeAnchorOffset( pos - creator->getCenter() ), farAnchorOffset( glm::vec2( 0.0f ) ), isAlive( GL_TRUE ),
	isAttached( GL_FALSE ), gameWidth( width ), timeLeft( duration ), PULL_STRENGTH( 2.0f ), anchorRadius( anchorRadius ) {

	// determine which textures to use based on launch direction
	if ( vel.x > 0 ) {
		// player 1 tech textures
		link = ResourceManager::getTexture( "tech_link" );
		anchor = ResourceManager::getTexture( "tech_anchor" );
		sticker = ResourceManager::getTexture( "tech_sticker" );
	} else {
		// player 2 alien textures
		link = ResourceManager::getTexture( "alien_link" );
		anchor = ResourceManager::getTexture( "alien_anchor" );
		sticker = ResourceManager::getTexture( "alien_sticker" );
	}

	// create a single sticker at the creating paddle
	endSticker = new BallObject( creator->getCenter() + glm::vec2( 2.0f * closeAnchorOffset.x, closeAnchorOffset.y ), anchorRadius, sticker, vel );
}

GrappleAttack::~GrappleAttack() {
	delete endSticker;
}

void GrappleAttack::update( GLfloat dt ) {
	if ( isAlive ) {
		// mark grapple as dead if hits edge of level or runs out of time
		if ( endSticker->pos.x > gameWidth || endSticker->pos.x < -endSticker->size.x || timeLeft <= 0.0f ) {
			isAlive = GL_FALSE;
			return;
		}

		if ( isAttached ) {
			timeLeft -= dt;

			// pull the target paddle toward the creator paddle vertically if attached
			GLfloat yDiff = creator->getCenter().y - endSticker->getCenter().y;
			target->pos.y += PULL_STRENGTH * yDiff * dt;

			endSticker->pos = target->getCenter() + farAnchorOffset;
		} else {
			// move the sticker
			endSticker->pos += endSticker->vel * dt;

			GLfloat yDiffPlayer1 = creator->getCenter().y - endSticker->getCenter().y;
			GLfloat yDiffPlayer2 = target->getCenter().y - endSticker->getCenter().y;
			GLfloat pullStrength = PULL_STRENGTH * yDiffPlayer2 * dt;

			// increase sticker vertical speed if both players are in the same direction with respect to the sticker
			if ( ( yDiffPlayer1 > 0 && yDiffPlayer2 > 0 ) || ( yDiffPlayer1 < 0 && yDiffPlayer2 < 0 ) ) {
				pullStrength *= 2;
			}
			endSticker->pos.y += pullStrength;
		}
	}
}

void GrappleAttack::draw( SpriteRenderer& renderer ) {

	// get angle from sticker to creator paddle
	glm::vec2 diff = creator->getCenter() - endSticker->getCenter();
	glm::vec2 normDiff = glm::normalize( diff );
	GLfloat angle = glm::degrees( std::atan2( diff.y, diff.x ) );
	angle = angle >= 0 ? angle : angle + 360;

	// render links
	glm::vec2 drawPos = endSticker->getCenter() + 2 * anchorRadius * normDiff;
	GLuint count = 0;
	while ( glm::dot( glm::normalize( creator->getCenter() - drawPos ), normDiff ) >= 0.0f ) {
		renderer.drawSprite( link, drawPos - glm::vec2( anchorRadius, anchorRadius / 2.0f ), glm::vec2( anchorRadius * 2.0f, anchorRadius ), angle );
		drawPos += normDiff * ( 2 * anchorRadius );
		count++;
		if ( count % NUM_LINKS_BETWEEN == 0 ) {
			drawPos += 2 * anchorRadius * normDiff;
		}
	}

	// render anchors
	drawPos = endSticker->getCenter();
	renderer.drawSprite( sticker, drawPos - anchorRadius, glm::vec2( anchorRadius * 2.2f ), angle );
	drawPos += normDiff * ( 2 * ( NUM_LINKS_BETWEEN + 1 ) * anchorRadius );
	while ( glm::dot( glm::normalize( creator->getCenter() - drawPos ), normDiff ) >= 0.0f ) {
		renderer.drawSprite( anchor, drawPos - anchorRadius, glm::vec2( anchorRadius * 2.0f ), angle );
		drawPos += normDiff * ( 2 * ( NUM_LINKS_BETWEEN + 1 ) * anchorRadius );
	}
}

void GrappleAttack::attachToTarget() {
	isAttached = GL_TRUE;
	farAnchorOffset = endSticker->pos - target->getCenter();
}