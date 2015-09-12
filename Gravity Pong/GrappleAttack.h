#pragma once

#ifndef	GRAPPLE_ATTACK_H
#define GRAPPLE_ATTACK_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "SpriteRenderer.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "BallObject.h"

class GrappleAttack {
public:
	static const GLuint		NUM_ANCHORS = 10;
	static const GLuint		NUM_LINKS = NUM_ANCHORS - 1;
	const GLfloat			PULL_STRENGTH;
	const GLfloat			TIGHTEN_VELOCITY = 300.0f;

	GameObject				links[NUM_LINKS];
	BallObject				anchors[NUM_ANCHORS];
	glm::vec2				closeAnchorOffset, farAnchorOffset;
	GameObject				*creator, *target;
	GLboolean				isAlive, isAttached;
	GLfloat					gameWidth, timeLeft;

							GrappleAttack( GameObject* creator, GameObject* target, glm::vec2 pos, GLfloat anchorRadius, glm::vec2 vel, GLuint width, GLfloat duration = 5.0f );
							~GrappleAttack();
	void					update( const GLfloat dt );
	void					draw( SpriteRenderer& renderer );
	void					attachToTarget();
};

#endif // GRAPPLE_ATTACK_H

