#include "PostProcessor.h"

#include <iostream>

// Reference: http://learnopengl.com/#!In-Practice/2D-Game/Postprocessing

PostProcessor::PostProcessor( GLuint width, GLuint height )
	: texture(), width( width ), height( height ), blind( GL_FALSE ), flip( GL_FALSE ), blindedPlayer( nullptr ) {

	// load shader effects
	defaultShader = ResourceManager::loadShader( "resources/shaders/pp_default.vs", "resources/shaders/pp_default.fs", nullptr, "defualtPostProcessor" );
	blindShader = ResourceManager::loadShader( "resources/shaders/pp_default.vs", "resources/shaders/pp_blind.fs", nullptr, "blindShader" );
	flipShader = ResourceManager::loadShader( "resources/shaders/pp_default.vs", "resources/shaders/pp_flip.fs", nullptr, "flipShader" );

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
	defaultShader.use();
	defaultShader.setInteger( "scene", 0 );
	blindShader.use();
	blindShader.setInteger( "scene", 0 );
	flipShader.use();
	flipShader.setInteger( "scene", 0 );
	flipShader.setFloat( "maxWidth", width );
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
	// setup appropriate post effects shader
	if ( blind ) {
		blindShader.use();
		blindShader.setVector2f( "playerPos", glm::vec2( blindedPlayer->getCenter().x, height - blindedPlayer->getCenter().y ) );
	} else if ( flip ) {
		flipShader.use();
	} else {
		defaultShader.use();
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

void PostProcessor::blindPlayer( const GameObject& player, GLfloat blindRange, glm::vec2 heightRange, glm::vec2 widthRange, GLfloat fadeInLine ) {
	clearEffects();
	blind = GL_TRUE;
	blindedPlayer = &player;
	blindShader.use();
	blindShader.setFloat( "fadeInLine", fadeInLine );
	blindShader.setVector2f( "widthRange", widthRange );
	blindShader.setVector2f( "heightRange", heightRange - heightRange.x );
	blindShader.setFloat( "visionRadius", blindRange );
}

void PostProcessor::flipScreen( glm::vec2 widthRange, glm::vec2 heightRange ) {
	clearEffects();
	flip = GL_TRUE;
	flipShader.use();
	flipShader.setVector2f( "widthRange", widthRange );
	flipShader.setVector2f( "heightRange", heightRange - heightRange.x );
}

void PostProcessor::clearEffects() {
	blind = GL_FALSE;
	flip = GL_FALSE;
	blindedPlayer = nullptr;
}