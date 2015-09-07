#pragma once

#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Texture.h"
#include "SpriteRenderer.h"

class GameObject {
public:
	glm::vec2 pos, size, vel;
	glm::vec3 color;
	GLfloat rotation, mass;
	GLboolean isSolid, isDestroyed;
	Texture sprite;

	GameObject();
	GameObject( glm::vec2 pos, glm::vec2 size, Texture sprite, glm::vec3 color = glm::vec3( 1.0f ), glm::vec2 vel = glm::vec2( 0.0f ) );
	~GameObject();

	glm::vec2 getCenter() const;
	virtual void draw(SpriteRenderer& renderer);
	void getVertices( glm::vec2 vertices[4] ) const;
};

#endif //GAME_OBJECT_H