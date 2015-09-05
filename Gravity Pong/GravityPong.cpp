#include "GravityPong.h"

#include <sstream>
#include <iostream>
#include <algorithm>

const float PI = 3.14159265358;

GravityPong::GravityPong( GLuint width, GLuint height )
	: Game( width, height ), state( GAME_OVER ), p1Lives( NUM_LIVES ), p2Lives( NUM_LIVES ), heightRange( 1.0f / 8.0f * height, height ), p1BounceCooldown( 0.0f ), p2BounceCooldown( 0.0f ),
	nextPunishmentCountdown( PUNISHMENT_COUNTDOWN ), GRAV_STARTING_RADIUS( height / 50.0f ), PADDLE_SPEED( ( height * ( 7.0f / 8.0f ) ) ), PADDLE_SIZE( width / 50.0f, height * ( 7.0f / 8.0f ) / 6.0f ) {
	init();
}

GravityPong::~GravityPong() {
	delete textRenderer;
	delete soundEngine;
	delete spriteRenderer;
	//delete particlesRenderer;
	delete postEffectsRenderer;
	delete player1;
	delete player2;
}

void GravityPong::init() {
	// load shaders
	ResourceManager::loadShader( "sprite.vs", "sprite.fs", nullptr, "sprite" );
	ResourceManager::loadShader( "particle.vs", "particle.fs", nullptr, "particle" );
	ResourceManager::loadShader( "post_effects.vs", "post_effects.fs", nullptr, "postProcessor" );

	// configure shaders
	glm::mat4 projection = glm::ortho( 0.0f, static_cast<GLfloat>( width ),
		static_cast<GLfloat>( height ), 0.0f, -1.0f, 1.0f );

	ResourceManager::getShader( "sprite" ).use().setInteger( "image", 0 );
	ResourceManager::getShader( "sprite" ).setMatrix4( "projection", projection );
	ResourceManager::getShader( "particle" ).use().setInteger( "sprite", 0 );
	ResourceManager::getShader( "particle" ).setMatrix4( "projection", projection );

	// load textures
	ResourceManager::loadTexture( "background.jpg", GL_FALSE, "background" );
	ResourceManager::loadTexture( "paddle.png", GL_TRUE, "paddle" );
	ResourceManager::loadTexture( "character.png", GL_TRUE, "ball" );
	ResourceManager::loadTexture( "arrow_green.png", GL_TRUE, "green_arrow" );
	ResourceManager::loadTexture( "gravity_ball.png", GL_TRUE, "gravity_ball" );
	ResourceManager::loadTexture( "black_color.png", GL_FALSE, "black" );
	ResourceManager::loadTexture( "green_color.png", GL_FALSE, "green" );
	ResourceManager::loadTexture( "light_blue_color.png", GL_FALSE, "light_blue" );
	ResourceManager::loadTexture( "red_color.png", GL_FALSE, "red" );
	ResourceManager::loadTexture( "white_color.png", GL_FALSE, "white" );

	// create player paddles
	glm::vec2 playerPos = glm::vec2( 0.0f, ( heightRange.y + heightRange.x ) / 2.0f - PADDLE_SIZE.y / 2.0f );
	glm::vec2 vel = glm::vec2( 0.0f, PADDLE_SPEED );
	player1 = new PaddleObject( playerPos, PADDLE_SIZE, glm::vec3( 1.0f ), ResourceManager::getTexture( "paddle" ), vel );
	playerPos = glm::vec2( width - PADDLE_SIZE.x, ( heightRange.y + heightRange.x ) / 2.0f - PADDLE_SIZE.y / 2.0f );
	player2 = new PaddleObject( playerPos, PADDLE_SIZE, glm::vec3( 1.0f ), ResourceManager::getTexture( "paddle" ), vel );

	// create ball
	GLuint radius = ( heightRange.y - heightRange.x ) / 50.0f;
	glm::vec2 ballPos = glm::vec2( width / 2.0f - radius, ( heightRange.y + heightRange.x ) / 2.0f - radius );
	ball = new GameBall( ballPos, radius, DEFAULT_BALL_SPEED, ResourceManager::getTexture( "ball" ) );
	ball->startLaunch();

	// create renderers
	spriteRenderer = new SpriteRenderer( ResourceManager::getShader( "sprite" ) );
	postEffectsRenderer = new PostProcessor( ResourceManager::getShader( "postProcessor" ), width, height );
	//particlesRenderer = new ParticleGenerator( ResourceManager::getShader( "particles" ),  );
	textRenderer = new TextRenderer( width, height );
	textRenderer->load( "ocraext.TTF", 24 );

	// create sound engine
	soundEngine = irrklang::createIrrKlangDevice();
	soundEngine->play2D( "breakout.mp3", GL_TRUE );
	soundEngine->setSoundVolume( 0.25f );

	// initialize player energy levels
	p1Energy = 1000.0f;
	p2Energy = 1000.0f;
}

void GravityPong::processInput( const GLfloat dt ) {
	if( state == GAME_ACTIVE ) {
		if( keys[GLFW_KEY_W] ) {
			player1->move( PADDLE_UP, dt, heightRange );
		}
		if( keys[GLFW_KEY_S] ) {
			player1->move( PADDLE_DOWN, dt, heightRange );
		}
		if( keys[GLFW_KEY_UP] ) {
			player2->move( PADDLE_UP, dt, heightRange );
		}
		if( keys[GLFW_KEY_DOWN] ) {
			player2->move( PADDLE_DOWN, dt, heightRange );
		}
		if( keys[GLFW_KEY_D] ) {
			if( p1ChargingGravBall == nullptr && p1Energy >= GRAV_BALL_COST ) {
				p1ChargingGravBall = new GravityBall( glm::vec2( player1->size.x, player1->getCenter().y - GRAV_STARTING_RADIUS ), GRAV_STARTING_RADIUS, ResourceManager::getTexture( "gravity_ball" ), GRAV_BALL_COST * 0.5f );
				p1Energy -= GRAV_BALL_COST;
			} else if( p1ChargingGravBall != nullptr ) {
				p1ChargingGravBall->growBall( dt, p1Energy );
				p1ChargingGravBall->pos = glm::vec2( player1->size.x, player1->getCenter().y - p1ChargingGravBall->radius );
			}
		}
		if( keys[GLFW_KEY_LEFT] ) {
			if( p2ChargingGravBall == nullptr && p2Energy >= GRAV_BALL_COST ) {
				p2ChargingGravBall = new GravityBall( glm::vec2( width - player2->size.x - GRAV_STARTING_RADIUS * 2.0f, player2->getCenter().y - GRAV_STARTING_RADIUS ), GRAV_STARTING_RADIUS, ResourceManager::getTexture( "gravity_ball" ), GRAV_BALL_COST * 0.5f );
				p2Energy -= GRAV_BALL_COST;
			} else if( p2ChargingGravBall != nullptr ) {
				p2ChargingGravBall->growBall( dt, p2Energy );
				p2ChargingGravBall->pos = glm::vec2( width - player2->size.x - p2ChargingGravBall->size.x, player2->getCenter().y - p2ChargingGravBall->radius );
			}
		}
		if( keys[GLFW_KEY_A] ) {
			// set the selected gravityBall to collapse
			GravityBall* gravPtr = findSelectedGravBall( P1_SELECTED );
			if( gravPtr != nullptr ) {
				gravPtr->isCollapsing = GL_TRUE;
				gravPtr->selectedBy = NO_ONE;
			}
		}
		if( keys[GLFW_KEY_RIGHT] ) {
			// set the selected gravityBall to collapse
			GravityBall* gravPtr = findSelectedGravBall( P2_SELECTED );
			if( gravPtr != nullptr ) {
				gravPtr->isCollapsing = GL_TRUE;
				gravPtr->selectedBy = NO_ONE;
			}
		}
	} else if( state == GAME_OVER ) {
		if( keys[GLFW_KEY_ENTER] ) {
			resetGame();
			state = GAME_ACTIVE;
		}
	}
}

void GravityPong::update( const GLfloat dt ) {
	if( state == GAME_ACTIVE ) {
		ball->update( dt, heightRange );

		handleCooldowns( dt );

		updateGravityBalls( dt );
		handleCollisions();

		// check to see if ball scored
		if( ball->pos.x < 0 || ball->pos.x + ball->size.x > width ) {
			if( ball->pos.x < 0 ) {
				--p1Lives;
			} else {
				--p2Lives;
			}
			// reset the ball or end the game
			if( p1Lives == 0 || p2Lives == 0 ) {
				state = GAME_OVER;
			} else {
				glm::vec2 ballPos = glm::vec2( width / 2.0f - ball->radius, ( heightRange.y - heightRange.x ) / 2.0f - ball->radius );
				ball->reset( ballPos, glm::vec2( 0.0f ) );
				clearPunishment();
				ball->startLaunch();
			}
		}

		// if ball is going too slow, launch it
		GLfloat speed = glm::length( ball->vel );
		if( !(punishment.type == OBUSE && punishment.charges > 0) && !ball->isLaunching && ( speed < MIN_BALL_SPEED || std::abs( ball->vel.x ) < MIN_BALL_SPEED_X ) ) {
			ball->startLaunch();
		} else if( speed > MAX_BALL_SPEED ) {
			// if ball is going to fast slow it down
			ball->vel = glm::vec2( glm::normalize( ball->vel ) * MAX_BALL_SPEED );
		}
	}
}

void GravityPong::render() {
	// begin rendering to post processing quad
	postEffectsRenderer->beginRender();

	spriteRenderer->drawSprite( ResourceManager::getTexture( "background" ), glm::vec2( 0, 0 ), glm::vec2( width, height ), 0.0f );

	// render launch arrow
	if( ball->isLaunching && ball->launchDT < 1.0f ) {
		glm::mat4 model;
		glm::vec2 size = glm::vec2( ball->size.x / 1.5f, ball->size.y / 2.0f );
		// translate (transformation order: scale, rotate, translate but in reversed order)
		model = glm::translate( model, glm::vec3( ball->pos.x + ball->size.x / 2.0f, ball->pos.y + ball->size.y / 2.0f, 0.0f ) );
		// rotate the arrow
		model = glm::rotate( model, ball->dir * 180.0f / PI, glm::vec3( 0.0f, 0.0f, 1.0f ) );
		// move the model to point of rotation
		model = glm::translate( model, glm::vec3( size.x, -size.y / 2.0f, 0.0f ) );
		// scale the model first
		model = glm::scale( model, glm::vec3( size, 1.0f ) );
		spriteRenderer->drawSprite( ResourceManager::getTexture( "green_arrow" ), model );
	}

	// render gravity balls
	if( p1ChargingGravBall != nullptr ) {
		p1ChargingGravBall->draw( *spriteRenderer );
	}
	if( p2ChargingGravBall != nullptr ) {
		p2ChargingGravBall->draw( *spriteRenderer );
	}
	for( GravityBall& gravBall : gravityBalls ) {
		gravBall.draw( *spriteRenderer );
		if( gravBall.selectedBy == P1_SELECTED ) {
			textRenderer->renderText( "P1", gravBall.pos.x, gravBall.pos.y, 1.0f, glm::vec3( 0.0f, 1.0f, 0.0f ) );
		} else if( gravBall.selectedBy == P2_SELECTED ) {
			textRenderer->renderText( "P2", gravBall.pos.x, gravBall.pos.y, 1.0f, glm::vec3( 0.5f, 0.85f, 0.85f ) );
		}
	}

	ball->draw( *spriteRenderer );
	player1->draw( *spriteRenderer );
	player2->draw( *spriteRenderer );

	renderGUI();

	// end rendering to post processing quad
	postEffectsRenderer->endRender();
	// render post processing quad
	postEffectsRenderer->render( glfwGetTime() );

	if( state == GAME_ACTIVE || state == GAME_OVER ) {
		std::stringstream ss;
		ss << p1Lives;
		textRenderer->renderText( "Lives: " + ss.str(), 5.0f, 5.0f, 1.0f );
		ss.clear();
		ss.str( std::string() );
		ss << p2Lives;
		textRenderer->renderText( "Lives: " + ss.str(), width - 130.0f, 5.0f, 1.0f );
	}

	if( state == GAME_OVER ) {
		textRenderer->renderText( p1Lives == 0 ? "Player 2 won" : "Player 1 won", width / 2 - 80.0f, height / 2, 1.0f );
		textRenderer->renderText( "Press ENTER to retry or ESC to quit", width / 2 - 170.0f, height / 2 + 20.0f, 0.75f );
	}
}

void GravityPong::handleCollisions() {
	// check for ball coliding with players
	Collision playerCollision = checkBallRectCollision( *ball, *player1 );
	PaddleObject* player = nullptr;
	if( std::get<0>( playerCollision ) ) {
		player = player1;

		// give player energy on bounce
		if( p1BounceCooldown <= 0.0f ) {
			p1Energy += ENERGY_PER_BOUNCE;
			p1BounceCooldown = BOUNCE_COOLDOWN_TIME;
		}

	} else {
		playerCollision = checkBallRectCollision( *ball, *player2 );
		if( std::get<0>( playerCollision ) ) {
			player = player2;

			// give player energy on bounce
			if( p2BounceCooldown <= 0.0f ) {
				p2Energy += ENERGY_PER_BOUNCE;
				p2BounceCooldown = BOUNCE_COOLDOWN_TIME;
			}
		}
	}
	// handle collision with player if there was one
	if( player != nullptr ) {
		int num = player == player1 ? 1 : 2;
		resolveBallPlayerCollision( *ball, *player, num );
	}

	// handle gravity ball player collisions
	for( GravityBall& gravBall : gravityBalls ) {

		// only detect collision if not collapsing
		if( !gravBall.isCollapsing ) {
			playerCollision = checkBallRectCollision( gravBall, *player1 );
			if( std::get<0>( playerCollision ) ) {
				resolveBallPlayerCollision( gravBall, *player1, 1 );

				// set grav ball to selected
				unselectGravBall( P1_SELECTED );
				gravBall.selectedBy = P1_SELECTED;

				// give player energy on bounce
				if( p1BounceCooldown <= 0.0f ) {
					p1Energy += ENERGY_PER_BOUNCE * ( gravBall.radius / gravBall.MAX_RADIUS );
					p1BounceCooldown = BOUNCE_COOLDOWN_TIME;
				}

			} else {
				playerCollision = checkBallRectCollision( gravBall, *player2 );
				if( std::get<0>( playerCollision ) ) {
					resolveBallPlayerCollision( gravBall, *player2, 2 );

					// set grav ball to selected
					unselectGravBall( P2_SELECTED );
					gravBall.selectedBy = P2_SELECTED;

					// give player energy on bounce
					if( p2BounceCooldown <= 0.0f ) {
						p2Energy += ENERGY_PER_BOUNCE * ( gravBall.radius / gravBall.MAX_RADIUS );
						p2BounceCooldown = BOUNCE_COOLDOWN_TIME;
					}
				}
			}
		}
	}

}

GLboolean GravityPong::checkRectRectCollision( const GameObject& one, const GameObject& two ) const {
	// collision with x axis
	bool collisionX = ( one.pos.x + one.size.x >= two.pos.x ) && ( two.pos.x + two.size.x >= one.pos.x );

	// colission with y axis
	bool collisionY = ( one.pos.y + one.size.y >= two.pos.y ) && ( two.pos.y + two.size.y >= one.pos.y );

	return collisionX && collisionY;
}

GLboolean GravityPong::checkBallBallCollision( const BallObject& one, const BallObject& two ) const {
	return glm::distance( one.pos, two.pos ) <= one.radius + two.radius;
}

Collision GravityPong::checkBallRectCollision( const BallObject& one, const GameObject& two ) const {
	glm::vec2 centerBall( one.pos + one.radius );
	glm::vec2 rectHalfExtents( two.size.x / 2, two.size.y / 2 );
	glm::vec2 centerRect( two.pos.x + rectHalfExtents.x, two.pos.y + rectHalfExtents.y );

	// get direction between ball and rect
	glm::vec2 direction = centerBall - centerRect;
	glm::vec2 clamped = glm::clamp( direction, -rectHalfExtents, rectHalfExtents );

	// find the closest point
	glm::vec2 closest = centerRect + clamped;
	direction = closest - centerBall;

	if( glm::length( direction ) <= one.radius ) {
		return std::make_tuple( GL_TRUE, vectorDirection( direction ), direction );
	} else {
		return std::make_tuple( GL_FALSE, UP, glm::vec2( 0, 0 ) );
	}
}

Direction GravityPong::vectorDirection( const glm::vec2 target ) const {
	glm::vec2 compass[] = {
		glm::vec2( 0.0f, 1.0f ),
		glm::vec2( 1.0f, 0.0f ),
		glm::vec2( 0.0f, -1.0f ),
		glm::vec2( -1.0f, 0.0f )
	};

	GLfloat max = 0.0f;
	Direction bestMatch = UP;
	for( GLuint i = 0; i < 4; ++i ) {
		GLfloat dotProduct = glm::dot( glm::normalize( target ), compass[i] );
		if( dotProduct > max ) {
			max = dotProduct;
			bestMatch = (Direction)i;
		}
	}
	return bestMatch;
}

void GravityPong::resetGame() {
	ball->reset( glm::vec2( width / 2.0f - ball->radius, ( heightRange.y + heightRange.x ) / 2.0f - ball->radius ), glm::vec2( 0.0f ) );
	ball->startLaunch();

	// reset paddles
	player1->vel = glm::vec2( 0.0f, PADDLE_SPEED );
	player2->size = PADDLE_SIZE;
	glm::vec2 playerPos = glm::vec2( 0.0f, ( heightRange.y + heightRange.x ) / 2.0f - PADDLE_SIZE.y / 2.0f );
	player1->reset( playerPos );
	playerPos = glm::vec2( width - PADDLE_SIZE.x, ( heightRange.y + heightRange.x ) / 2.0f - PADDLE_SIZE.y / 2.0f );
	player2->reset( playerPos );
	p1Energy = p2Energy = 1000.0f;
	p1Lives = NUM_LIVES;
	p2Lives = NUM_LIVES;

	// clear gravity balls
	gravityBalls.clear();

	// reset punishment
	clearPunishment();
}

void GravityPong::resolveBallPlayerCollision( BallObject& ball, const PaddleObject player, const int num ) {
	// check where the ball hit the player, and change vel based on location
	GLfloat centerBoard = player.pos.y + player.size.y / 2;
	GLfloat distance = ( ball.pos.y + ball.radius ) - centerBoard;
	GLfloat percentage = distance / ( player.size.y / 2 );
	GLfloat angle = ( percentage * MAX_HIT_ANGLE ) * PI / 180.0f;
	GLfloat oldSpeed = glm::length( ball.vel );

	// move accordingly
	ball.vel.x = oldSpeed * std::cos( angle );
	ball.vel.y = oldSpeed * std::sin( angle );
	if( num == 2 ) {
		ball.vel.x = -ball.vel.x;
	}
}

void GravityPong::updateGravityBalls( const GLfloat dt ) {
	// update gravity balls
	if( p1ChargingGravBall != nullptr ) {
		p1ChargingGravBall->update( dt, heightRange );
		if( !keys[GLFW_KEY_D] ) {
			unselectGravBall( P1_SELECTED );

			// launch the gravity ball
			p1ChargingGravBall->vel = glm::vec2( p1ChargingGravBall->speed, 0.0f );
			p1ChargingGravBall->color = glm::vec3( 1.0f );
			p1ChargingGravBall->selectedBy = P1_SELECTED;
			gravityBalls.push_back( *p1ChargingGravBall );

			delete p1ChargingGravBall;
			p1ChargingGravBall = nullptr;
		}
	}
	if( p2ChargingGravBall != nullptr ) {
		p2ChargingGravBall->update( dt, heightRange );
		if( !keys[GLFW_KEY_LEFT] ) {
			unselectGravBall( P2_SELECTED );

			// launch the gravity ball
			p2ChargingGravBall->vel = glm::vec2( -p2ChargingGravBall->speed, 0.0f );
			p2ChargingGravBall->color = glm::vec3( 1.0f );
			p2ChargingGravBall->selectedBy = P2_SELECTED;
			gravityBalls.push_back( *p2ChargingGravBall );

			delete p2ChargingGravBall;
			p2ChargingGravBall = nullptr;
		}
	}
	for( GravityBall& gravBall : gravityBalls ) {
		gravBall.update( dt, heightRange );
		gravBall.pullObject( dt, *ball );
	}

	// remove gravity balls that are out of bounds
	gravityBalls.erase( std::remove_if( gravityBalls.begin(), gravityBalls.end(), [&]( const GravityBall& gravBall ) {
		return ( ( gravBall.pos.x < -gravBall.size.x ) || ( gravBall.pos.x > width ) || gravBall.radius <= 0.0f );
	} ), gravityBalls.end() );
}

void GravityPong::renderGUI() const {
	// draw gui background
	spriteRenderer->drawSprite( ResourceManager::getTexture( "black" ), glm::vec2( 0.0f ), glm::vec2( width, heightRange.x ), 0.0f, glm::vec3( 1.0f ) );
	spriteRenderer->drawSprite( ResourceManager::getTexture( "light_blue" ), glm::vec2( 0.0f, 0.97f * heightRange.x ), glm::vec2( width, 0.03 * heightRange.x ), 0.0f, glm::vec3( 1.0f ) );

	// draw player energy bars
	GLfloat offsetX = 0.10f, offsetY = 0.02f;
	glm::vec2 pos = glm::vec2( offsetX * width, offsetY * height );
	glm::vec2 size = glm::vec2( ( width * ( 1.0f - 2.0f * offsetX ) ) * ( (GLfloat)p1Energy / ( p1Energy + p2Energy ) ), height * 0.03f );

	spriteRenderer->drawSprite( ResourceManager::getTexture( "green" ), pos, size, 0.0f, glm::vec3( 0.75f ) );
	glm::vec2 pos2 = glm::vec2( pos.x + size.x, offsetY * height );
	glm::vec2 size2 = glm::vec2( ( width * ( 1.0f - 2.0f * offsetX ) ) * ( (GLfloat)p2Energy / ( p1Energy + p2Energy ) ), height * 0.03f );
	spriteRenderer->drawSprite( ResourceManager::getTexture( "light_blue" ), pos2, size2, 0.0f, glm::vec3( 0.75f ) );

	// draw player energy text
	std::stringstream ss;
	ss << (GLuint)p1Energy;
	textRenderer->renderText( ss.str(), pos.x + width * 0.01f, pos.y + height * 0.005f, 1.5f, glm::vec3( 0.0f ) );
	ss.str( std::string() );
	ss << (GLuint)p2Energy;
	textRenderer->renderText( ss.str(), pos2.x + size2.x - width * 0.06f, pos.y + height * 0.005f, 1.5f, glm::vec3( 0.0f ) );

	// draw punishment notifications
	offsetX = 0.40f;
	pos = glm::vec2( offsetX * width, pos.y + size.y + heightRange.x * 0.03f );
	size = glm::vec2( ( width * ( 1.0f - 2.0f * offsetX ) ), 0.5f * heightRange.x );
	spriteRenderer->drawSprite( ResourceManager::getTexture( "red" ), pos, size, 0.0f, glm::vec3( 0.75f ) );
	size.y = 0.3f * size.y;
	spriteRenderer->drawSprite( ResourceManager::getTexture( "white" ), pos, size, 0.0f, glm::vec3( 0.75f ) );
	textRenderer->renderText( "Punishment", width / 2.0f - 0.03f * width, pos.y + 0.01f * heightRange.x, 1.0f, glm::vec3( 0.0f ) );

	if( nextPunishmentCountdown > 0.0f ) {
		// draw punishment countdown
		ss.str( std::string() );
		ss << (GLint)nextPunishmentCountdown;
		textRenderer->renderText( ss.str(), width / 2.0f - 0.01f * width, pos.y + 0.2f * heightRange.x, 2.0f );
	} else {
		// draw current punishment
		pos.x = pos.x + 0.01f * heightRange.x;
		pos.y = pos.y + 0.01f * heightRange.x + size.y;
		size.x = 0.05f * width;
		size.y = 0.32f * heightRange.x;

		// draw which player is selected
		spriteRenderer->drawSprite( ResourceManager::getTexture( "white" ), pos, size, 0.0f, glm::vec3( 1.0f ) );
		ss.str( std::string() );
		ss << (GLint)punishment.player + 1;
		textRenderer->renderText( "P" + ss.str(), pos.x + size.x / 3.0f, pos.y + size.y / 5.0f, 1.5f, glm::vec3( 0.0f ) );

		// draw what the punishment is
		pos.x += 0.074 * width;
		spriteRenderer->drawSprite( ResourceManager::getTexture( "white" ), pos, size, 0.0f, glm::vec3( 1.0f ) );
		switch( punishment.type ) {
		case SLOW:
			textRenderer->renderText( "Slow", pos.x + size.x / 20.0f, pos.y + size.y / 5.0f, 1.5f, glm::vec3( 0.0f ) );
			break;
		case SHRINK:
			textRenderer->renderText( "Shrink", pos.x + size.x / 20.0f, pos.y + size.y / 3.0f, 1.0f, glm::vec3( 0.0f ) );
			break;
		case OBUSE:
			textRenderer->renderText( "Obuse", pos.x + size.x / 20.0f, pos.y + size.y / 4.0f, 1.2f, glm::vec3( 0.0f ) );
			break;
		}

		// draw its duration
		pos.x += 0.074 * width;
		spriteRenderer->drawSprite( ResourceManager::getTexture( "white" ), pos, size, 0.0f, glm::vec3( 1.0f ) );
		ss.str( std::string() );
		if( punishment.charges > 0 ) {
			ss << "C:" << punishment.charges;
		} else {
			ss << "T:" << (GLint)punishment.timeLeft;
		}
		textRenderer->renderText( ss.str(), pos.x + size.x / 20.0f, pos.y + size.y / 5.0f, 1.5f, glm::vec3( 0.0f ) );
	}

}

void GravityPong::unselectGravBall( const PLAYER_SELECTED player ) {
	if( player == NO_ONE ) {
		return;
	}

	GravityBall* gravBallPtr = findSelectedGravBall( player );
	if( gravBallPtr != nullptr ) {
		gravBallPtr->selectedBy = NO_ONE;
	}
}

GravityBall* GravityPong::findSelectedGravBall( const PLAYER_SELECTED player ) {
	if( player == NO_ONE ) {
		return nullptr;
	}

	for( GravityBall& gravBall : gravityBalls ) {
		if( gravBall.selectedBy == player ) {
			return &gravBall;
		}
	}
}

void GravityPong::handleCooldowns( const GLfloat dt ) {
	// reduce cooldowns
	if( p1BounceCooldown > 0.0f ) {
		p1BounceCooldown -= dt;
	}
	if( p2BounceCooldown > 0.0f ) {
		p2BounceCooldown -= dt;
	}
	if( nextPunishmentCountdown > 0.0f && punishment.player == NO_ONE ) {
		nextPunishmentCountdown -= dt;
		// apply punishment if cooldown reaches 0
		if( nextPunishmentCountdown <= 0.0f ) {
			dealPunishment();
		}
	} else if( punishment.player != NO_ONE && punishment.timeLeft > 0.0f || punishment.charges > 0 ) {
		if( punishment.timeLeft > 0.0f ) {
			punishment.timeLeft -= dt;

			// undo punishment
			if( punishment.timeLeft <= 0.0f ) {
				clearPunishment();
			}
		} else {

			if( !ball->isLaunching ) {
				--punishment.charges;
				ball->startLaunch( punishment.player );
			}


			if( punishment.charges == 0 ) {
				switch( punishment.type ) {
				case OBUSE:
					punishment.timeLeft = PRE_LAUNCH_TIME;
					break;
				}
			}
		}
	}
}

void GravityPong::dealPunishment() {
	// select player to punish based on energy Levels
	int random = rand() % 100 + 1;
	PLAYER_SELECTED selectedPlayer = NO_ONE;
	PaddleObject* paddle = nullptr;
	if( random <= 100 * ( 1.0f - ( p1Energy / ( p1Energy + p2Energy ) ) ) ) {
		selectedPlayer = P1_SELECTED;
		paddle = player1;
	} else {
		selectedPlayer = P2_SELECTED;
		paddle = player2;
	}

	// randomly selected the punishment
	random = rand() % 3;
	PUNISHMENT_TYPE type = (PUNISHMENT_TYPE) random;
	punishment = Punishment( selectedPlayer, type, paddle );
	switch( type ) {
	case SLOW:
		punishment.paddle->vel = glm::vec2( 0.0f, PADDLE_SPEED * 0.5f );
		break;
	case SHRINK:
		paddle->pos.y += 0.25f * PADDLE_SIZE.y;
		punishment.paddle->size.y = PADDLE_SIZE.y * SHRINK_AMOUNT;
		break;
	case OBUSE:
		ball->startLaunch( punishment.player );
		--punishment.charges;
		break;
	}
}

void GravityPong::clearPunishment() {
	if( punishment.player != NO_ONE ) {
		switch( punishment.type ) {
		case SLOW:
			punishment.paddle->vel = glm::vec2( 0.0f, PADDLE_SPEED );
			break;
		case SHRINK:
			punishment.paddle->pos.y -= SHRINK_AMOUNT / 2.0f * PADDLE_SIZE.y;
			punishment.paddle->size.y = PADDLE_SIZE.y;
			break;
		}

		punishment = Punishment();
	}
	nextPunishmentCountdown = PUNISHMENT_COUNTDOWN;
}
