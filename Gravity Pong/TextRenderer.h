#pragma once

#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <map>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Texture.h"
#include "Shader.h"

enum TEXT_ALIGNMENT {
	LEFT_ALIGNED,
	RIGHT_ALIGNED,
	CENTERED
};

struct Character {
	GLuint textureID;
	glm::ivec2 size; // size of glyph
	glm::ivec2 bearing; // offset from baseline to left/top of glyph
	GLuint advance; // horizontal offset to advance to next glyph
};

class TextRenderer {
public:
	std::map<GLchar, Character> characters;
	Shader						shader;

								TextRenderer( GLuint width, GLuint height );
								~TextRenderer();
	void						load( std::string font, GLuint fontSize );
	void						renderText( std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color = glm::vec3( 1.0f ), TEXT_ALIGNMENT alignment = LEFT_ALIGNED );
private:
	GLuint VAO, VBO;
};

#endif TEXT_RENDERER_H
