#pragma once

// Reference: http://www.learnopengl.com/#!In-Practice/2D-Game/Rendering-Sprites

#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include "GL/glew.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Texture.h"

class SpriteRenderer {
public:
	SpriteRenderer(Shader& shader);
	~SpriteRenderer();
	void drawSprite(Texture& texture, glm::vec2 position, glm::vec2 size = glm::vec2(10, 10), GLfloat rotate = 0.0f, glm::vec4 color = glm::vec4(1.0f), GLboolean flipY = GL_FALSE );
	void drawSprite( Texture& texture, glm::mat4 model, glm::vec4 color = glm::vec4( 1.0f ) );
private:
	Shader shader;
	GLuint VAO;
	void initRenderData();
};

#endif //SPRITE_RENDERER_H
