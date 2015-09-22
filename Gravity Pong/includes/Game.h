#pragma once

#ifndef GAME_H
#define GAME_H

#include <GL/glew.h>

class Game {
public:
	// keyboard events
	GLboolean	keys[1024], keysProcessed[1024];

	// game resolution
	GLuint			width, height;

					Game( GLuint width, GLuint height );
	virtual			~Game();
	virtual void	init() = 0;
	virtual void	processInput( const GLfloat dt ) = 0;
	virtual void	update( const GLfloat dt ) = 0;
	virtual void	render() = 0;
};

#endif // GAME_H
