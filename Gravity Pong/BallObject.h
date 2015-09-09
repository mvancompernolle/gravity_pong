#pragma once

#ifndef BALL_OBJECT_H
#define BALL_OBJECT_H

#include <GL/glew.h>

#include <glm/glm.hpp>

#include "Texture.h"
#include "SpriteRenderer.h"
#include "GameObject.h"

class BallObject : public GameObject {
public:
	GLfloat			radius;

					BallObject();
					BallObject(const glm::vec2 pos, const GLfloat radius, const Texture sprite, const glm::vec2 vel = glm::vec2( 0.0f ) );
					~BallObject();
};

#endif //BALL_OBJECT_H

