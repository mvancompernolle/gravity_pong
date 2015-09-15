#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "ResourceManager.h"
#include "GravityPong.h"

void keyCallBack( GLFWwindow* window, int key, int scanCode, int action, int mode );

// declare callbacks for GLFW
GLuint SCREEN_WIDTH = 1920;
GLuint SCREEN_HEIGHT = 1080;
const GLboolean FULL_SCREEN = GL_TRUE;

Game* game = nullptr;

int main( int argc, char* argv[] ) {
	// initialize glfw
	GLFWwindow* window = nullptr;
	glfwInit();
	/*glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
	//glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );*/
	glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );

	if( FULL_SCREEN ) {
		GLFWvidmode* mode = (GLFWvidmode*)glfwGetVideoMode( glfwGetPrimaryMonitor() );
		glfwWindowHint( GLFW_RED_BITS, mode->redBits );
		glfwWindowHint( GLFW_GREEN_BITS, mode->greenBits );
		glfwWindowHint( GLFW_BLUE_BITS, mode->blueBits );
		glfwWindowHint( GLFW_REFRESH_RATE, mode->refreshRate );

		SCREEN_WIDTH = mode->width;
		SCREEN_HEIGHT = mode->height;
		window = glfwCreateWindow( SCREEN_WIDTH, SCREEN_HEIGHT, "Gravity Pong", glfwGetPrimaryMonitor(), nullptr );
	} else {
		window = glfwCreateWindow( SCREEN_WIDTH, SCREEN_HEIGHT, "Gravity Pong", nullptr, nullptr );
	}
	glfwMakeContextCurrent( window );
	glfwSwapInterval( 1 );
	

	glewExperimental = GL_TRUE;
	glewInit();
	// check to see if there was a glewInit() bug
	glGetError();

	glfwSetKeyCallback( window, keyCallBack );

	// opengl config
	glViewport( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT );
	glEnable( GL_CULL_FACE );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	// initialize the game
	game = new GravityPong( SCREEN_WIDTH, SCREEN_HEIGHT );
	game->init();

	// time variables
	GLfloat dt = 0.0f, lastTime = 0.0f;

	RetroRenderer renderer;

	// start the game within the menu state
	while( !glfwWindowShouldClose( window ) ) {
		// calculate dt
		GLfloat currentTime = glfwGetTime();
		dt = currentTime - lastTime;
		lastTime = currentTime;
		glfwPollEvents();

		// keep dt small
		if( dt > 0.333f ) {
			dt = 0.333f;
		}

		// process input
		game->processInput( dt );

		// update game state
		game->update( dt );

		// render the game
		glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT );

		game->render();

		glfwSwapBuffers( window );
	}

	ResourceManager::clear();

	//system( "pause" );

	glfwDestroyWindow( window );
	glfwTerminate();
	return 0;
}

void keyCallBack( GLFWwindow* window, int key, int scanCode, int action, int mode ) {
	if( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS ) {
		glfwSetWindowShouldClose( window, GL_TRUE );
	}
	if( key >= 0 && key < 1024 ) {
		if( action == GLFW_PRESS ) {
			game->keys[key] = GL_TRUE;
		} else if( action == GLFW_RELEASE ) {
			game->keys[key] = GL_FALSE;
			game->keysProcessed[key] = GL_FALSE;
		}
	}
}