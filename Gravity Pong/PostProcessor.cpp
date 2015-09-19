#include "PostProcessor.h"

#include <iostream>

PostProcessor::PostProcessor( Shader shader, GLuint width, GLuint height )
	: shader( shader ), texture(), width( width ), height( height ), blind( GL_FALSE ), blindedPlayer( nullptr ) {
	// initialize FBO's
	glGenFramebuffers( 1, &MSFBO );
	glGenFramebuffers( 1, &FBO );
	glGenRenderbuffers( 1, &RBO );

	// initialize renderbuffer storage with a multisampled color buffer
	glBindFramebuffer( GL_FRAMEBUFFER, MSFBO );
	glBindRenderbuffer( GL_RENDERBUFFER, RBO );
	// allocate storage space for render buffer object
	glRenderbufferStorageMultisample( GL_RENDERBUFFER, 4, GL_RGB, width, height );
	// attach MS render muffer object to framebuffer
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, RBO );
	if ( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
		std::cout << "ERROR::POSTPROCESSOR: Failed to initialize MSFBO" << std::endl;

	// initialize the FBO/texture to blit multisampled color-buffer to; used for shader operations (for postprocessing effects)
	glBindFramebuffer( GL_FRAMEBUFFER, FBO );
	texture.generate( width, height, NULL );
	// attach texture to framebuffer as its color attachment
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.ID, 0 );
	if ( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
		std::cout << "ERROR::POSTPROCESSOR: Failed to initialize FBO" << std::endl;
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	// initialize render data and uniforms
	initRenderData();
	shader.use();
	shader.setInteger( "scene", 0 );
}

PostProcessor::~PostProcessor() {

}

void PostProcessor::beginRender() {
	glBindFramebuffer( GL_FRAMEBUFFER, MSFBO );
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
}

void PostProcessor::endRender() {
	// now resolve multisampled color-buffer into intermediate FBO to store to texture
	glBindFramebuffer( GL_READ_FRAMEBUFFER, MSFBO );
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, FBO );
	glBlitFramebuffer( 0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST );
	// Binds both READ and WRITE framebuffer to default framebuffer
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void PostProcessor::render( GLfloat time ) {
	// Set uniforms/options
	shader.use();
	shader.setInteger( "blind", blind );
	if ( blind ) {
		shader.setVector2f( "playerPos", glm::vec2( blindedPlayer->getCenter().x, height - blindedPlayer->getCenter().y ) );
	}
	// render textured quad
	glActiveTexture( GL_TEXTURE0 );
	texture.bind();
	glBindVertexArray( VAO );
	glDrawArrays( GL_TRIANGLES, 0, 6 );
	glBindVertexArray( 0 );
}

void PostProcessor::initRenderData() {
	// configure VAO/VBO
	GLuint VBO;
	GLfloat vertices[] = {
		// Pos        // Tex
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 0.0f, 1.0f,

		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	};
	glGenVertexArrays( 1, &VAO );
	glGenBuffers( 1, &VBO );

	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );

	glBindVertexArray( VAO );
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof( GL_FLOAT ), (GLvoid*)0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindVertexArray( 0 );
}

void PostProcessor::blindPlayer( const GameObject& player, GLfloat blindRange, glm::vec2 heightRange ) {
	blind = GL_TRUE;
	blindedPlayer = &player;
	shader.setVector2f( "heightRange", heightRange - heightRange.x );
	shader.setFloat( "visionRadius", blindRange );
}

void PostProcessor::clearEffects() {
	blind = GL_FALSE;
	blindedPlayer = nullptr;
}