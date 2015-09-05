#include "PostProcessor.h"

#include <iostream>

PostProcessor::PostProcessor( Shader shader, GLuint width, GLuint height ) 
	: shader( shader ), texture(), width( width ), height( height ), confuse( GL_FALSE ), chaos( GL_FALSE ), shake( GL_FALSE ) {
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
	if( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
		std::cout << "ERROR::POSTPROCESSOR: Failed to initialize MSFBO" << std::endl;

	// initialize the FBO/texture to blit multisampled color-buffer to; used for shader operations (for postprocessing effects)
	glBindFramebuffer( GL_FRAMEBUFFER, FBO );
	texture.generate( width, height, NULL );
	// attach texture to framebuffer as its color attachment
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.ID, 0 );
	if( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
		std::cout << "ERROR::POSTPROCESSOR: Failed to initialize FBO" << std::endl;
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	// initialize render data and uniforms
	initRenderData();
	shader.use();
	shader.setInteger( "scene", 0 );
	GLfloat offset = 1.0f / 300.0f;
	GLfloat offsets[9][2] = {
			{ -offset, offset },  // top-left
			{ 0.0f, offset },  // top-center
			{ offset, offset },  // top-right
			{ -offset, 0.0f },  // center-left
			{ 0.0f, 0.0f },  // center-center
			{ offset, 0.0f },  // center - right
			{ -offset, -offset },  // bottom-left
			{ 0.0f, -offset },  // bottom-center
			{ offset, -offset }   // bottom-right    
	};
	glUniform2fv( glGetUniformLocation( shader.ID, "offsets" ), 9, (GLfloat*)offsets );
	GLint edgeKernel[9] = {
		-1, -1, -1,
		-1, 8, -1,
		-1, -1, -1
	};
	glUniform1iv( glGetUniformLocation( this->shader.ID, "edgeKernel" ), 9, edgeKernel );
	GLfloat blurKernel[9] = {
		1.0 / 16, 2.0 / 16, 1.0 / 16,
		2.0 / 16, 4.0 / 16, 2.0 / 16,
		1.0 / 16, 2.0 / 16, 1.0 / 16
	};
	glUniform1fv( glGetUniformLocation( this->shader.ID, "blurKernel" ), 9, blurKernel );
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
	shader.setFloat( "time", time );
	shader.setInteger( "confuse", confuse );
	shader.setInteger( "chaos", chaos );
	shader.setInteger( "shake", shake );
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