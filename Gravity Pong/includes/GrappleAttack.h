#pragma once

#ifndef	GRAPPLE_ATTACK_H
#define GRAPPLE_ATTACK_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>

#include "SpriteRenderer.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "BallObject.h"

class GrappleAttack {
public:
	static const GLuint		NUM_LINKS_BETWEEN = 5;
	const GLfloat			PULL_STRENGTH;
	const GLfloat			TIGHTEN_VELOCITY = 300.0f;

	BallObject*				endSticker;
	glm::vec2				closeAnchorOffset, farAnchorOffset;
	GameObject				*creator, *target;
	GLboolean				isAlive, isAttached;
	GLfloat					gameWidth, timeLeft, anchorRadius;
	Texture					anchor, link, sticker;

							GrappleAttack( GameObject* creator, GameObject* target, glm::vec2 pos, GLfloat anchorRadius, glm::vec2 vel, GLuint width, GLfloat duration = 5.0f );
							~GrappleAttack();
	void					update( GLfloat dt );
	void					draw( SpriteRenderer& renderer );
	void					attachToTarget();
};

#endif // GRAPPLE_ATTACK_H

