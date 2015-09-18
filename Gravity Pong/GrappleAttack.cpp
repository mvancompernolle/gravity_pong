#include "GrappleAttack.h"
#include <iostream>

GrappleAttack::GrappleAttack(GameObject* creator, GameObject* target, glm::vec2 pos, GLfloat anchorRadius, glm::vec2 vel, GLuint width, GLfloat duration)
	: creator(creator), target(target), closeAnchorOffset(pos - creator->getCenter()), farAnchorOffset(glm::vec2(0.0f)), isAlive(GL_TRUE),
	isAttached(GL_FALSE), gameWidth(width), timeLeft(duration), PULL_STRENGTH(2.0f), anchorRadius( anchorRadius ) {

	if (vel.x > 0) {
		link = ResourceManager::getTexture("tech_link");
		anchor = ResourceManager::getTexture("tech_anchor");
		sticker = ResourceManager::getTexture("tech_sticker");
	}
	else {
		link = ResourceManager::getTexture("alien_link");
		anchor = ResourceManager::getTexture("alien_anchor");
		sticker = ResourceManager::getTexture("alien_sticker");
	}

	// create a single anchar at the creating paddle
	anchors.push_back(BallObject(creator->getCenter() + glm::vec2(2.0f * closeAnchorOffset.x, closeAnchorOffset.y), anchorRadius, sticker, vel));
}

GrappleAttack::~GrappleAttack() {

}

void GrappleAttack::update(const GLfloat dt) {
	if (isAlive) {
		// destory grapple if hits edge of level or runs out of time
		if (anchors.front().pos.x > gameWidth || anchors.front().pos.x < -anchors.front().size.x || timeLeft <= 0.0f) {
			isAlive = GL_FALSE;
			return;
		}

		if (isAttached) {
			timeLeft -= dt;

			// pull the target paddle toward the creator paddle vertically if attached
			GLfloat yDiff = creator->getCenter().y - anchors.front().getCenter().y;
			target->pos.y += PULL_STRENGTH * yDiff * dt;

			anchors.front().pos = target->getCenter() + farAnchorOffset;
		}
		else {
			// move the end anchor
			anchors.front().pos += anchors.front().vel * dt;

			GLfloat yDiffPlayer1 = creator->getCenter().y - anchors.front().getCenter().y;
			GLfloat yDiffPlayer2 = target->getCenter().y - anchors.front().getCenter().y;
			GLfloat pullStrength = PULL_STRENGTH * yDiffPlayer2 * dt;
			if ((yDiffPlayer1 > 0 && yDiffPlayer2 > 0) || (yDiffPlayer1 < 0 && yDiffPlayer2 < 0)) {
				pullStrength *= 2;
			}
			anchors.front().pos.y += pullStrength;
		}
	}
}

void GrappleAttack::draw(SpriteRenderer& renderer) {

	glm::vec2 diff = creator->getCenter() - anchors.front().getCenter();
	glm::vec2 normDiff = glm::normalize(diff);
	GLfloat angle = glm::degrees(std::atan2(diff.y, diff.x));
	angle = angle >= 0 ? angle : angle + 360;

	// render links
	glm::vec2 drawPos = anchors.front().getCenter() + 2 * anchorRadius * normDiff;
	GLuint count = 0;
	while (glm::dot(glm::normalize(creator->getCenter() - drawPos), normDiff) >= 0.0f) {
		renderer.drawSprite(link, drawPos - glm::vec2(anchorRadius, anchorRadius / 2.0f), glm::vec2(anchorRadius * 2.0f, anchorRadius), angle);
		drawPos += normDiff * (2 * anchorRadius);
		count++;
		if (count % NUM_LINKS_BETWEEN == 0) {
			drawPos += 2 * anchorRadius * normDiff;
		}
	}

	// render anchors
	drawPos = anchors.front().getCenter();
	renderer.drawSprite(sticker, drawPos - anchorRadius, glm::vec2(anchorRadius * 2.2f), angle);
	drawPos += normDiff * (2 * (NUM_LINKS_BETWEEN + 1) * anchorRadius) ;
	while ( glm::dot(glm::normalize(creator->getCenter() - drawPos), normDiff) >= 0.0f ) {
		renderer.drawSprite(anchor, drawPos - anchorRadius, glm::vec2(anchorRadius * 2.0f), angle);
		drawPos += normDiff * (2 * (NUM_LINKS_BETWEEN + 1) * anchorRadius);
	}
}

void GrappleAttack::attachToTarget() {
	isAttached = GL_TRUE;
	farAnchorOffset = anchors.front().pos - target->getCenter();
}