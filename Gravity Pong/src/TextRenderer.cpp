#include "TextRenderer.h"

// Reference: http://www.learnopengl.com/#!In-Practice/2D-Game/Render-text

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "TextRenderer.h"
#include "ResourceManager.h"

TextRenderer::TextRenderer( GLuint width, GLuint height ) {
	// load and configure shader
	shader = ResourceManager::loadShader( "resources/shaders/text.vs", "resources/shaders/text.fs", nullptr, "text" );
	shader.use().setMatrix4( "projection", glm::ortho( 0.0f, static_cast<GLfloat>( width ), static_cast<GLfloat>( height ), 0.0f ) );
	shader.setInteger( "text", 0 );
	// configure VAO/VBO for texture quads
	glGenVertexArrays( 1, &VAO );
	glGenBuffers( 1, &VBO );
	glBindVertexArray( VAO );
	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	glBufferData( GL_ARRAY_BUFFER, sizeof( GLfloat ) * 6 * 4, NULL, GL_DYNAMIC_DRAW );
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof( GLfloat ), 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindVertexArray( 0 );
}

TextRenderer::~TextRenderer() {

}

void TextRenderer::load( std::string font, GLuint fontSize ) {
	// clear the previously loaded characters
	characters.clear();
	// initialize and load the freetype library
	FT_Library ft;
	if( FT_Init_FreeType( &ft ) ) {
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	}
	// load font as face
	FT_Face face;
	if( FT_New_Face( ft, font.c_str(), 0, &face ) ) {
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	}
	// set size of glyph
	FT_Set_Pixel_Sizes( face, 0, fontSize );
	// disable byte-alignment restriction
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	// for the first 128 ASCII character, pre-load/compile their characters and store them
	for( GLubyte c = 0; c < 128; c++ ) {
		// load character glyph
		if( FT_Load_Char( face, c, FT_LOAD_RENDER ) ) {
			std::cout << "ERROR::FREETYPE: Failed to laod Glyph" << std::endl;
			continue;
		}
		// generate texture
		GLuint texture;
		glGenTextures( 1, &texture );
		glBindTexture( GL_TEXTURE_2D, texture );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer );
		// set texture options
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		// store the character for later use
		Character character = {
			texture,
			glm::ivec2( face->glyph->bitmap.width, face->glyph->bitmap.rows ),
			glm::ivec2( face->glyph->bitmap_left, face->glyph->bitmap_top ),
			face->glyph->advance.x
		};
		characters.insert( std::pair<GLchar, Character>( c, character ) );
	}
	glBindTexture( GL_TEXTURE_2D, 0 );
	// destroy freetype
	FT_Done_Face( face );
	FT_Done_FreeType( ft );
}

void TextRenderer::renderText( std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color, TEXT_ALIGNMENT alignment ) {
	// activate corresponding render state
	shader.use();
	shader.setVector3f( "textColor", color );
	glActiveTexture( GL_TEXTURE0 );
	glBindVertexArray( VAO );

	if( alignment == RIGHT_ALIGNED || alignment == CENTERED) {
		// offset x with size of text
		GLint offsetX =  ( text.length() ) * ( characters[text[0]].advance >> 6 ) * scale;
		x -= alignment == RIGHT_ALIGNED ?  offsetX : offsetX / 2.0f;
	}

	// iterate through all characters
	std::string::const_iterator c;
	for ( int c = 0; c < text.length(); ++c ) {
		Character ch = characters[text[c]];

		GLfloat xpos = x + ch.bearing.x * scale;
		GLfloat ypos = y + ( characters['H'].bearing.y - ch.bearing.y ) * scale;

		GLfloat w = ch.size.x * scale;
		GLfloat h = ch.size.y * scale;

		// update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos, ypos + h, 0.0, 1.0 },
			{ xpos + w, ypos, 1.0, 0.0 },
			{ xpos, ypos, 0.0, 0.0 },

			{ xpos, ypos + h, 0.0, 1.0 },
			{ xpos + w, ypos + h, 1.0, 1.0 },
			{ xpos + w, ypos, 1.0, 0.0 }
		};
		// render glyph texture over quad
		glBindTexture( GL_TEXTURE_2D, ch.textureID );
		// update content of VBO memory
		glBindBuffer( GL_ARRAY_BUFFER, VBO );
		glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( vertices ), vertices );

		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glDrawArrays( GL_TRIANGLES, 0, 6 );
		// now advance cursors for next glyph
		x += ( ch.advance >> 6 ) * scale;
	}
	glBindVertexArray( 0 );
	glBindTexture( GL_TEXTURE_2D, 0 );
}
