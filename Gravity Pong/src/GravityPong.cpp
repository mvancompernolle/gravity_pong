#include "GravityPong.h"

#include <sstream>
#include <iostream>
#include <algorithm>

const float PI = 3.14159265358;

GravityPong::GravityPong( GLuint width, GLuint height )
	: Game( width, height ), state( GAME_OVER ), p1Lives( NUM_LIVES ), p2Lives( NUM_LIVES ), heightRange( GUI_PERCENT * height, height ), p1BounceCooldown( 0.0f ), p2BounceCooldown( 0.0f ),
	nextPunishmentCountdown( PUNISHMENT_COUNTDOWN ), GRAV_STARTING_RADIUS( height / 50.0f ), PADDLE_SPEED( ( height ) ), PADDLE_SIZE( width / 50.0f, height * ( 7.0f / 8.0f ) / 6.0f ),
	MISSILE_SIZE( height / 15.0f, height / 30.0f ), p1MissileCooldown( 0.0f ), p2MissileCooldown( 0.0f ), inRetroMode( GL_FALSE ),
	LEECH_SPEED( width ), nextPowerUpCooldown( 3.0f ), POWERUP_RADIUS( ( GUI_PERCENT * height + height ) / 40.0f ), currentMaxBallSpeed( START_MAX_BALL_SPEED ) {

	init();
}

GravityPong::~GravityPong() {
	delete textRenderer;
	delete spriteRenderer;
	delete particlesRenderer;
	delete postEffectsRenderer;
	delete player1;
	delete player2;
	delete ball;

	if ( p1ChargingGravBall != nullptr ) {
		delete p1ChargingGravBall;
	}
	if ( p2ChargingGravBall != nullptr ) {
		delete p2ChargingGravBall;
	}
	if ( p1Missile != nullptr ) {
		delete p1Missile;
	}
	if ( p2Missile != nullptr ) {
		delete p2Missile;
	}
	if ( p1Grapple != nullptr ) {
		delete p1Grapple;
	}
	if ( p2Grapple != nullptr ) {
		delete p2Grapple;
	}

	soundEngine->drop();
	ResourceManager::clear();
}

void GravityPong::init() {
	// load shaders
	ResourceManager::loadShader( "resources/shaders/sprite.vs", "resources/shaders/sprite.fs", nullptr, "sprite" );

	// configure shaders
	glm::mat4 projection = glm::ortho( 0.0f, static_cast<GLfloat>( width ),
		static_cast<GLfloat>( height ), 0.0f, -1.0f, 1.0f );
	ResourceManager::getShader( "sprite" ).use().setInteger( "image", 0 );
	ResourceManager::getShader( "sprite" ).setMatrix4( "projection", projection );

	// load textures
	ResourceManager::loadTexture( "resources/images/background.jpg", GL_TRUE, "background" );
	ResourceManager::loadTexture( "resources/images/alien_paddle.png", GL_TRUE, "alien_paddle" );
	ResourceManager::loadTexture( "resources/images/tech_paddle.png", GL_TRUE, "tech_paddle" );
	ResourceManager::loadTexture( "resources/images/ball.png", GL_TRUE, "ball" );
	ResourceManager::loadTexture( "resources/images/arrow_green.png", GL_TRUE, "green_arrow" );
	ResourceManager::loadTexture( "resources/images/gravity_ball.png", GL_TRUE, "gravity_ball" );
	ResourceManager::loadTexture( "resources/images/repulsion_ball.png", GL_TRUE, "repulsion_ball" );
	ResourceManager::loadTexture( "resources/images/tech_missile.png", GL_TRUE, "tech_missile" );
	ResourceManager::loadTexture( "resources/images/alien_missile.png", GL_TRUE, "alien_missile" );
	ResourceManager::loadTexture( "resources/images/alien_explosion.png", GL_TRUE, "alien_explosion" );
	ResourceManager::loadTexture( "resources/images/tech_explosion.png", GL_TRUE, "tech_explosion" );
	ResourceManager::loadTexture( "resources/images/smoke.png", GL_TRUE, "smoke" );
	ResourceManager::loadTexture( "resources/images/alien_leech.png", GL_TRUE, "alien_leech" );
	ResourceManager::loadTexture( "resources/images/tech_leech.png", GL_TRUE, "tech_leech" );
	ResourceManager::loadTexture( "resources/images/alien_anchor.png", GL_TRUE, "alien_anchor" );
	ResourceManager::loadTexture( "resources/images/alien_link.png", GL_TRUE, "alien_link" );
	ResourceManager::loadTexture( "resources/images/alien_sticker.png", GL_TRUE, "alien_sticker" );
	ResourceManager::loadTexture( "resources/images/tech_anchor.png", GL_TRUE, "tech_anchor" );
	ResourceManager::loadTexture( "resources/images/tech_link.png", GL_TRUE, "tech_link" );
	ResourceManager::loadTexture( "resources/images/tech_sticker.png", GL_TRUE, "tech_sticker" );
	ResourceManager::loadTexture( "resources/images/black_color.png", GL_FALSE, "black" );
	ResourceManager::loadTexture( "resources/images/green_color.png", GL_FALSE, "green" );
	ResourceManager::loadTexture( "resources/images/purple_color.png", GL_FALSE, "purple" );
	ResourceManager::loadTexture( "resources/images/light_blue_color.png", GL_FALSE, "light_blue" );
	ResourceManager::loadTexture( "resources/images/red_color.png", GL_FALSE, "red" );
	ResourceManager::loadTexture( "resources/images/white_color.png", GL_FALSE, "white" );

	// load gui textures
	ResourceManager::loadTexture( "resources/images/gui_background.png", GL_TRUE, "gui_background" );
	ResourceManager::loadTexture( "resources/images/life.png", GL_TRUE, "life" );
	ResourceManager::loadTexture( "resources/images/energy.png", GL_TRUE, "energy" );
	ResourceManager::loadTexture( "resources/images/black_punishment_box.png", GL_TRUE, "black_punishment_box" );
	ResourceManager::loadTexture( "resources/images/punishment_text_box.png", GL_TRUE, "punishment_text_box" );
	ResourceManager::loadTexture( "resources/images/punishment_template.png", GL_TRUE, "punishment_template" );

	// load punishment icons
	ResourceManager::loadTexture( "resources/images/slow_punishment.png", GL_TRUE, "slow_punishment" );
	ResourceManager::loadTexture( "resources/images/shrink_punishment.png", GL_TRUE, "shrink_punishment" );
	ResourceManager::loadTexture( "resources/images/abuse_punishment.png", GL_TRUE, "abuse_punishment" );
	ResourceManager::loadTexture( "resources/images/inverse_punishment.png", GL_TRUE, "inverse_punishment" );
	ResourceManager::loadTexture( "resources/images/trail_punishment.png", GL_TRUE, "trail_punishment" );
	ResourceManager::loadTexture( "resources/images/blind_punishment.png", GL_TRUE, "blind_punishment" );
	ResourceManager::loadTexture( "resources/images/flip_punishment.png", GL_TRUE, "flip_punishment" );

	// power up icons
	ResourceManager::loadTexture("resources/images/leech_powerup.png", GL_TRUE, "leech_powerup");
	ResourceManager::loadTexture("resources/images/missile_powerup.png", GL_TRUE, "missile_powerup");
	ResourceManager::loadTexture("resources/images/grapple_powerup.png", GL_TRUE, "grapple_powerup");
	ResourceManager::loadTexture("resources/images/leech_powerup_icon.png", GL_TRUE, "leech_powerup_icon");
	ResourceManager::loadTexture("resources/images/missile_powerup_icon.png", GL_TRUE, "missile_powerup_icon");
	ResourceManager::loadTexture("resources/images/grapple_powerup_icon.png", GL_TRUE, "grapple_powerup_icon");

	// create player paddles
	glm::vec2 playerPos = glm::vec2( 0.0f, ( heightRange.y + heightRange.x ) / 2.0f - PADDLE_SIZE.y / 2.0f );
	player1 = new PaddleObject( playerPos, PADDLE_SIZE, glm::vec4( 1.0f ), ResourceManager::getTexture( "tech_paddle" ), PADDLE_SPEED );
	playerPos = glm::vec2( width - PADDLE_SIZE.x, ( heightRange.y + heightRange.x ) / 2.0f - PADDLE_SIZE.y / 2.0f );
	player2 = new PaddleObject( playerPos, PADDLE_SIZE, glm::vec4( 1.0f ), ResourceManager::getTexture( "alien_paddle" ), PADDLE_SPEED );

	// create ball
	GLuint radius = ( heightRange.y - heightRange.x ) / 50.0f;
	glm::vec2 ballPos = glm::vec2( width / 2.0f - radius, ( heightRange.y + heightRange.x ) / 2.0f - radius );
	ball = new GameBall( ballPos, radius, DEFAULT_BALL_SPEED, ResourceManager::getTexture( "ball" ) );
	ball->startLaunch();

	// create renderers
	spriteRenderer = new SpriteRenderer( ResourceManager::getShader( "sprite" ) );
	postEffectsRenderer = new PostProcessor( width, height );
	particlesRenderer = new ParticleGenerator( ResourceManager::getTexture( "smoke" ), 500, 20.0f );
	textRenderer = new TextRenderer( width, height );
	textRenderer->load( "resources/fonts/ocraext.TTF", 24 );

	// create sound engine
	soundEngine = irrklang::createIrrKlangDevice();
	soundEngine->play2D( "resources/sounds/breakout.mp3", GL_TRUE );
	soundEngine->setSoundVolume( GAME_VOLUME );

	// initialize player energy levels
	p1Energy = p2Energy = STARTING_ENERGY;

	// randomly select first punishment
	int randomPunishment = rand() % NUM_PUNISHMENTS;
	nextPunishmentType = (PUNISHMENT_TYPE)randomPunishment;
}

void GravityPong::processInput( const GLfloat dt ) {
	// space key to toggle between normal and retro render mode
	if ( keys[GLFW_KEY_SPACE] && !keysProcessed[GLFW_KEY_SPACE] ) {
		keysProcessed[GLFW_KEY_SPACE] = GL_TRUE;
		inRetroMode = !inRetroMode;
	}

	// p to pause
	if ( keys[GLFW_KEY_P] && !keysProcessed[GLFW_KEY_P] ) {
		if ( state == GAME_PAUSED ) {
			soundEngine->setAllSoundsPaused( GL_FALSE );
			state = GAME_ACTIVE;
		} else if ( state == GAME_ACTIVE ) {
			state = GAME_PAUSED;
			soundEngine->setAllSoundsPaused( GL_TRUE );
		}
		keysProcessed[GLFW_KEY_P] = GL_TRUE;
	}

	// player controls during game
	if ( state == GAME_ACTIVE ) {
		// movement keys
		if ( keys[GLFW_KEY_W] ) {
			player1->move( PADDLE_UP );
		}
		if ( keys[GLFW_KEY_S] ) {
			player1->move( PADDLE_DOWN );
		}
		if ( keys[GLFW_KEY_HOME] ) {
			player2->move( PADDLE_UP );
		}
		if ( keys[GLFW_KEY_END] ) {
			player2->move( PADDLE_DOWN );
		}



		// used for gravity balls
		if ( keys[GLFW_KEY_D] ) {
			if ( p1ChargingGravBall == nullptr && p1Energy >= GRAV_BALL_COST ) {
				Texture ballTex = ResourceManager::getTexture( "gravity_ball" );
				p1ChargingGravBall = new GravityBall( glm::vec2( player1->size.x, player1->getCenter().y - GRAV_STARTING_RADIUS ), GRAV_STARTING_RADIUS, ballTex, GRAV_BALL_COST * 0.5f );
				p1Energy -= GRAV_BALL_COST;

				// play gravity sound
				p1ChargingGravBall->sound = soundEngine->play2D( "resources/sounds/gravity_sound.wav", GL_TRUE, GL_FALSE, GL_TRUE );
				p1ChargingGravBall->sound->setVolume( 0.5f );

			} else if ( p1ChargingGravBall != nullptr ) {
				p1ChargingGravBall->growBall( dt, p1Energy );
				p1ChargingGravBall->pos = glm::vec2( player1->size.x, player1->getCenter().y - p1ChargingGravBall->radius );
			}
		}
		if ( keys[GLFW_KEY_DELETE] ) {
			if ( p2ChargingGravBall == nullptr && p2Energy >= GRAV_BALL_COST ) {
				Texture ballTex = ResourceManager::getTexture( "gravity_ball" );
				p2ChargingGravBall = new GravityBall( glm::vec2( width - player2->size.x - GRAV_STARTING_RADIUS * 2.0f, player2->getCenter().y - GRAV_STARTING_RADIUS ), GRAV_STARTING_RADIUS, ballTex, GRAV_BALL_COST * 0.5f );
				p2Energy -= GRAV_BALL_COST;

				// play gravity sound
				p2ChargingGravBall->sound = soundEngine->play2D( "resources/sounds/gravity_sound.wav", GL_TRUE, GL_FALSE, GL_TRUE );
				p2ChargingGravBall->sound->setVolume( 0.5f );

			} else if ( p2ChargingGravBall != nullptr ) {
				p2ChargingGravBall->growBall( dt, p2Energy );
				p2ChargingGravBall->pos = glm::vec2( width - player2->size.x - p2ChargingGravBall->size.x, player2->getCenter().y - p2ChargingGravBall->radius );
			}
		}
		// used for repulsion balls
		if (keys[GLFW_KEY_E]) {
			if (p1ChargingGravBall == nullptr && p1Energy >= GRAV_BALL_COST) {
				Texture ballTex = ResourceManager::getTexture("repulsion_ball");
				p1ChargingGravBall = new GravityBall(glm::vec2(player1->size.x, player1->getCenter().y - GRAV_STARTING_RADIUS), GRAV_STARTING_RADIUS, ballTex, GRAV_BALL_COST * 0.5f);
				p1ChargingGravBall->setReversed(GL_TRUE);
				p1Energy -= GRAV_BALL_COST;

				// play gravity sound
				p1ChargingGravBall->sound = soundEngine->play2D("resources/sounds/gravity_sound.wav", GL_TRUE, GL_FALSE, GL_TRUE);
				p1ChargingGravBall->sound->setVolume(0.5f);

			}
			else if (p1ChargingGravBall != nullptr) {
				p1ChargingGravBall->growBall(dt, p1Energy);
				p1ChargingGravBall->pos = glm::vec2(player1->size.x, player1->getCenter().y - p1ChargingGravBall->radius);
			}
		}
		if (keys[GLFW_KEY_INSERT]) {
			if (p2ChargingGravBall == nullptr && p2Energy >= GRAV_BALL_COST) {
				Texture ballTex = ResourceManager::getTexture("repulsion_ball");
				p2ChargingGravBall = new GravityBall(glm::vec2(width - player2->size.x - GRAV_STARTING_RADIUS * 2.0f, player2->getCenter().y - GRAV_STARTING_RADIUS), GRAV_STARTING_RADIUS, ballTex, GRAV_BALL_COST * 0.5f);
				p2ChargingGravBall->setReversed(GL_TRUE);
				p2Energy -= GRAV_BALL_COST;

				// play gravity sound
				p2ChargingGravBall->sound = soundEngine->play2D("resources/sounds/gravity_sound.wav", GL_TRUE, GL_FALSE, GL_TRUE);
				p2ChargingGravBall->sound->setVolume(0.5f);

			}
			else if (p2ChargingGravBall != nullptr) {
				p2ChargingGravBall->growBall(dt, p2Energy);
				p2ChargingGravBall->pos = glm::vec2(width - player2->size.x - p2ChargingGravBall->size.x, player2->getCenter().y - p2ChargingGravBall->radius);
			}
		}
		if (keys[GLFW_KEY_A]) {
			// set the selected gravityBall to collapse
			GravityBall* gravPtr = findSelectedGravBall(P1_SELECTED);
			if (gravPtr != nullptr) {
				gravPtr->isCollapsing = GL_TRUE;
				gravPtr->selectedBy = NO_ONE;
			}
		}
		if (keys[GLFW_KEY_PAGE_DOWN]) {
			// set the selected gravityBall to collapse
			GravityBall* gravPtr = findSelectedGravBall(P2_SELECTED);
			if (gravPtr != nullptr) {
				gravPtr->isCollapsing = GL_TRUE;
				gravPtr->selectedBy = NO_ONE;
			}
		}

		// player 1 secondary moves (powerups)
		if (keys[GLFW_KEY_Q] && !keysProcessed[GLFW_KEY_Q] ) {
			if (p1PowerUp.charges > 0) {
				switch (p1PowerUp.type) {
				case MISSILE:
					if (p1Missile == nullptr && p1MissileCooldown <= 0.0f) {
						--p1PowerUp.charges;
						p1Missile = new Missile(glm::vec2(player1->pos.x + 1.5f * player1->size.x, player1->getCenter().y - MISSILE_SIZE.y / 2.0f), MISSILE_SIZE, ResourceManager::getTexture("tech_missile"), ball, 0.0f, *soundEngine);
						p1Missile->setBoundaries(0.0f, width, heightRange.x, heightRange.y);
					}
					else if (p1Missile != nullptr) {
						causeMissileExplosion(*p1Missile);
						deleteMissile(p1Missile);
					}
					break;
				case LEECH:
					--p1PowerUp.charges;
					leechAttacks.push_back(LeechAttack(glm::vec2(player1->pos.x + 1.5f * player1->size.x - LEECH_SIZE.x / 2.0f, player1->getCenter().y - LEECH_SIZE.x / 2.0f),
						LEECH_SIZE, ResourceManager::getTexture("tech_leech"), glm::vec2(LEECH_SPEED, 0.0f), player2));

					// play leech launch sound
					soundEngine->play2D("resources/sounds/leech_launch.mp3", GL_FALSE);
					break;
				case GRAPPLE:
					if (p1Grapple == nullptr) {
						--p1PowerUp.charges;
						glm::vec2 pos = glm::vec2(player1->getCenter().x + player1->size.x / 2.0f - GRAPPLE_ANCHOR_RADIUS, player1->getCenter().y - GRAPPLE_ANCHOR_RADIUS);
						p1Grapple = new GrappleAttack(player1, player2, pos, GRAPPLE_ANCHOR_RADIUS, glm::vec2(GRAPPLE_SPEED, 0.0f), width, GRAPPLE_DURATION);

						// play grapple sound
						soundEngine->play2D("resources/sounds/grapple_release.wav", GL_FALSE);
					}
					break;
				}
				keysProcessed[GLFW_KEY_Q] = GL_TRUE;
			}

			if (!keysProcessed[GLFW_KEY_Q]) {
				switch (p1PowerUp.type) {
				case MISSILE:
					if (p1Missile != nullptr) {
						causeMissileExplosion(*p1Missile);
						deleteMissile(p1Missile);
					}
					break;
				}
			}
			
		}
		// player 2 secondary moves (powerups)
		if (keys[GLFW_KEY_PAGE_UP] && !keysProcessed[GLFW_KEY_PAGE_UP] ) {
			if (p2PowerUp.charges > 0) {
				switch (p2PowerUp.type) {
				case MISSILE:
					if (p2Missile == nullptr && p2MissileCooldown <= 0.0f) {
						--p2PowerUp.charges;
						p2Missile = new Missile(glm::vec2(player2->pos.x - 0.5f * player2->size.x - MISSILE_SIZE.x, player2->getCenter().y - MISSILE_SIZE.y / 2.0f), MISSILE_SIZE, ResourceManager::getTexture("alien_missile"), ball, 180.0f, *soundEngine, 180.0f);
						p2Missile->setBoundaries(0.0f, width, heightRange.x, heightRange.y);
					}
					else if (p2Missile != nullptr) {
						causeMissileExplosion(*p2Missile);
						deleteMissile(p2Missile);
					}
					break;
				case LEECH:
					--p2PowerUp.charges;
					leechAttacks.push_back(LeechAttack(glm::vec2(player2->pos.x - 0.5f * player2->size.x - LEECH_SIZE.x / 2.0f, player2->getCenter().y - LEECH_SIZE.x / 2.0f),
						LEECH_SIZE, ResourceManager::getTexture("alien_leech"), glm::vec2(-LEECH_SPEED, 0.0f), player1));

					// play leech launch sound
					soundEngine->play2D("resources/sounds/leech_launch.mp3", GL_FALSE);
					break;
				case GRAPPLE:
					if (p1Grapple == nullptr) {
						--p2PowerUp.charges;
						glm::vec2 pos = glm::vec2(player2->pos.x - GRAPPLE_ANCHOR_RADIUS, player2->getCenter().y - GRAPPLE_ANCHOR_RADIUS);
						p2Grapple = new GrappleAttack(player2, player1, pos, GRAPPLE_ANCHOR_RADIUS, glm::vec2(-GRAPPLE_SPEED, 0.0f), width, GRAPPLE_DURATION);

						// play grapple sound
						soundEngine->play2D("resources/sounds/grapple_release.wav", GL_FALSE);
					}
					break;
				}
				keysProcessed[GLFW_KEY_PAGE_UP] = GL_TRUE;
			}

			if (!keysProcessed[GLFW_KEY_PAGE_UP]) {
				switch (p2PowerUp.type) {
				case MISSILE:
					if (p2Missile != nullptr) {
						causeMissileExplosion(*p2Missile);
						deleteMissile(p2Missile);
					}
					break;
				}
			}

		}

	} else if ( state == GAME_OVER ) {
		// enter to start game
		if ( keys[GLFW_KEY_ENTER] ) {
			resetGame();
			state = GAME_ACTIVE;
		}
	}
}

void GravityPong::update( const GLfloat dt ) {
	// don't update if game is paused
	if ( state == GAME_PAUSED )
		return;

	if ( state == GAME_ACTIVE ) {
		// give players energy
		GLfloat energy = ENERGY_PER_SECOND * dt;
		addEnergy( P1_SELECTED, energy );
		addEnergy( P2_SELECTED, energy );

		// update the players and ball
		player1->update( dt, heightRange );
		player2->update( dt, heightRange );
		ball->update( dt, heightRange, *soundEngine );

		handleCooldowns( dt );
		updateGravityBalls( dt );

		// update explosions
		for ( Explosion& explosion : explosions ) {
			explosion.update( dt );
		}
		explosions.erase( std::remove_if( explosions.begin(), explosions.end(), [&] ( const Explosion& explosion ) {
			return explosion.timeLeft <= 0.0f;
		} ), explosions.end() );

		// update missiles if they exist and generate smoke aprticles
		if ( p1Missile != nullptr ) {
			p1Missile->update( dt );
			glm::vec2 rect[4];
			p1Missile->getVertices( rect );
			particlesRenderer->addParticles( ( rect[0] + rect[3] ) / 2.0f - particlesRenderer->particleSize / 2.0f, 3 );
		}
		if ( p2Missile != nullptr ) {
			p2Missile->update( dt );
			glm::vec2 rect[4];
			p2Missile->getVertices( rect );
			particlesRenderer->addParticles( ( rect[0] + rect[3] ) / 2.0f - particlesRenderer->particleSize / 2.0f, 3, glm::vec3( 0.6f, 0.0f, 0.8f ) );
		}
		particlesRenderer->update( dt );

		// update grapples if they exist
		if ( p1Grapple != nullptr ) {
			p1Grapple->update( dt );
		}
		if ( p2Grapple != nullptr ) {
			p2Grapple->update( dt );
		}

		for ( LeechAttack& leech : leechAttacks ) {
			leech.update( dt );
		}

		// update powerups
		for ( PowerUp& powerUp : powerUps ) {
			powerUp.update( dt );
		}

		// check for collisions between most objects
		handleCollisions();

		// check to see if ball scored
		if ( ball->pos.x < 0 || ball->pos.x + ball->size.x > width ) {
			if ( ball->pos.x < 0 ) {
				--p1Lives;

				// play life lost sound
				soundEngine->play2D( "resources/sounds/life_lost.wav", GL_FALSE );
			} else {
				--p2Lives;

				// play life lost sound
				soundEngine->play2D( "resources/sounds/life_lost.wav", GL_FALSE );
			}
			// reset the ball or end the game
			if ( p1Lives == 0 || p2Lives == 0 ) {
				state = GAME_OVER;
			} else {
				// if life is lost but game is not over, reset ball position and clear current punishment
				glm::vec2 ballPos = glm::vec2( width / 2.0f - ball->radius, ( heightRange.y + heightRange.x ) / 2.0f - ball->radius );
				ball->reset( ballPos, glm::vec2( 0.0f ) );
				clearPunishment();
				currentMaxBallSpeed = START_MAX_BALL_SPEED;
				ball->startLaunch();
			}
		}

		// if ball is going too slow, launch it
		GLfloat speed = glm::length( ball->vel );
		std::cout << "speed: " << speed << std::endl;
		if (!(punishment.type == ABUSE && punishment.charges > 0) && !ball->isLaunching) {
			if (std::abs(ball->vel.x) < MIN_BALL_SPEED_X) {
				slowTimeTillLaunch -= dt;
				if (slowTimeTillLaunch <= 0) {
					slowTimeTillLaunch = BALL_LAUNCH_THRESHOLD;
					ball->startLaunch();
				}
			}
			else {
				slowTimeTillLaunch = BALL_LAUNCH_THRESHOLD;
			}
		}
		if ( speed > currentMaxBallSpeed ) {
			// if ball is going to fast slow it down
			ball->vel = glm::vec2( glm::normalize( ball->vel ) * currentMaxBallSpeed );
		}

		// speed up max ball speed
		if (currentMaxBallSpeed < END_MAX_BALL_SPEED) {
			std::cout << "max: " << currentMaxBallSpeed << std::endl;
			currentMaxBallSpeed += BALL_MAX_SPEED_INCREASE_RATE * dt;
		}
	}
}

void GravityPong::render() {

	if ( !inRetroMode ) {
		renderNormal();
	} else {
		glUseProgram( 0 );
		renderRetro();
	}

}

void GravityPong::renderNormal() {
	// begin rendering to post processing quad
	postEffectsRenderer->beginRender();

	spriteRenderer->drawSprite( ResourceManager::getTexture( "background" ), glm::vec2( 0, 0 ), glm::vec2( width, height ), 0.0f );

	// render launch arrow
	if ( ball->isLaunching && ball->launchDT < 1.0f ) {
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
	if ( p1ChargingGravBall != nullptr ) {
		p1ChargingGravBall->draw( *spriteRenderer );
	}
	if ( p2ChargingGravBall != nullptr ) {
		p2ChargingGravBall->draw( *spriteRenderer );
	}
	for ( GravityBall& gravBall : gravityBalls ) {
		gravBall.draw( *spriteRenderer );
		if ( gravBall.selectedBy == P1_SELECTED ) {
			textRenderer->renderText( "P1", gravBall.pos.x, gravBall.pos.y, 1.0f, glm::vec3( 0.57f, 0.80f, 0.97f ) );
		} else if ( gravBall.selectedBy == P2_SELECTED ) {
			textRenderer->renderText( "P2", gravBall.pos.x, gravBall.pos.y, 1.0f, glm::vec3( 0.72f, 0.63f, 1.00f ) );
		}
	}

	// render missiles and particles
	particlesRenderer->draw( *spriteRenderer );
	if ( p1Missile != nullptr ) {
		p1Missile->draw( *spriteRenderer );
	}
	if ( p2Missile != nullptr ) {
		p2Missile->draw( *spriteRenderer );
	}

	// render grapples
	if ( p1Grapple != nullptr ) {
		p1Grapple->draw( *spriteRenderer );
	}
	if ( p2Grapple != nullptr ) {
		p2Grapple->draw( *spriteRenderer );
	}

	// render players and ball
	ball->draw( *spriteRenderer );
	player1->draw( *spriteRenderer );
	player2->draw( *spriteRenderer );

	// render leeches
	for ( LeechAttack& leech : leechAttacks ) {
		leech.draw( *spriteRenderer );
	}

	// draw explosions on top
	for ( Explosion& explosion : explosions ) {
		explosion.draw( *spriteRenderer );
	}

	// render powerups
	for ( PowerUp& powerUp : powerUps ) {
		powerUp.draw( *spriteRenderer );
	}

	// render gui
	renderGUI();

	// end rendering to post processing quad
	postEffectsRenderer->endRender();
	// render post processing quad
	postEffectsRenderer->render( glfwGetTime() );
}

void GravityPong::renderRetro() {

	// render gravity balls
	if ( p1ChargingGravBall != nullptr ) {
		retroRenderer.renderGravityBall( *p1ChargingGravBall );
	}
	if ( p2ChargingGravBall != nullptr ) {
		retroRenderer.renderGravityBall( *p2ChargingGravBall );
	}
	for ( GravityBall& gravBall : gravityBalls ) {
		retroRenderer.renderGravityBall( gravBall );
		if ( gravBall.selectedBy == P1_SELECTED ) {
			retroRenderer.renderPlayerSymbol( P1_SELECTED, gravBall.pos - gravBall.radius * 0.75f );
		} else if ( gravBall.selectedBy == P2_SELECTED ) {
			retroRenderer.renderPlayerSymbol( P2_SELECTED, gravBall.pos - gravBall.radius * 0.75f );
		}
	}

	// render missiles
	if ( p1Missile != nullptr ) {
		retroRenderer.renderMissile( *p1Missile );
	}
	if ( p2Missile != nullptr ) {
		retroRenderer.renderMissile( *p2Missile );
	}
	// render particles
	std::vector<Particle> particles = particlesRenderer->getParticles();
	GLfloat radius = particlesRenderer->particleSize / 2.0f;
	for ( Particle& particle : particles ) {
		if ( particle.color.a > 0.5f && particle.life >= 0.0f ) {
			retroRenderer.renderCircle( particle.pos + radius, radius, 8 );
		}
	}

	// render explosions
	for ( Explosion& explosion : explosions ) {
		retroRenderer.renderCircle( explosion.getCenter(), explosion.radius, 20 );
	}

	// render leech attacks
	for ( LeechAttack& leech : leechAttacks ) {
		retroRenderer.renderLeech( leech );
	}

	// render grapples
	if ( p1Grapple != nullptr ) {
		retroRenderer.renderGrapple( *p1Grapple );
	}
	if ( p2Grapple != nullptr ) {
		retroRenderer.renderGrapple( *p2Grapple );
	}

	// render paddles and game ball
	retroRenderer.renderGameBall( *ball );
	retroRenderer.renderPaddle( *player1 );
	retroRenderer.renderPaddle( *player2 );

	// render gui
	renderGUIRetro();
}

void GravityPong::handleCollisions() {
	// check for ball coliding with players
	Collision playerCollision = checkBallRectCollision( *ball, *player1 );
	PaddleObject* player = nullptr;
	if ( std::get<0>( playerCollision ) ) {
		player = player1;

		// player bounce sound
		soundEngine->play2D( "resources/sounds/tech_bounce.wav", GL_FALSE );

		// give player energy on bounce
		if ( p1BounceCooldown <= 0.0f ) {
			addEnergy( P1_SELECTED, ENERGY_PER_BOUNCE );
			p1BounceCooldown = BOUNCE_COOLDOWN_TIME;
		}

	} else {
		playerCollision = checkBallRectCollision( *ball, *player2 );
		if ( std::get<0>( playerCollision ) ) {
			player = player2;

			// player bounce sound
			irrklang::ISound* sound = soundEngine->play2D( "resources/sounds/alien_bounce.wav", GL_FALSE, GL_TRUE, GL_TRUE );
			sound->setVolume( 1.5f );
			sound->setIsPaused( GL_FALSE );

			// give player energy on bounce
			if ( p2BounceCooldown <= 0.0f ) {
				addEnergy( P2_SELECTED, ENERGY_PER_BOUNCE );
				p2BounceCooldown = BOUNCE_COOLDOWN_TIME;
			}
		}
	}
	// handle ball collision with player if there was one
	if ( player != nullptr ) {
		int num = player == player1 ? 1 : 2;
		resolveBallPlayerCollision( *ball, *player, num );
	}

	// handle missile collisions
	if ( p1Missile != nullptr ) {
		GLboolean wasCollision = false;
		Collision collision = checkBallRectCollision( *ball, *p1Missile );
		if ( std::get<0>( collision ) ) {
			wasCollision = true;
		} else if ( checkRectRectCollision( *player1, *p1Missile ) || checkRectRectCollision( *player2, *p1Missile ) || checkWallsRectCollision( *p1Missile ) ) {
			wasCollision = true;
		}
		if ( wasCollision ) {
			causeMissileExplosion( *p1Missile );
			deleteMissile( p1Missile );
		}
	}
	if ( p2Missile != nullptr ) {
		GLboolean wasCollision = false;
		Collision collision = checkBallRectCollision( *ball, *p2Missile );
		if ( std::get<0>( collision ) ) {
			wasCollision = true;
		} else if ( checkRectRectCollision( *player1, *p2Missile ) || checkRectRectCollision( *player2, *p2Missile ) || checkWallsRectCollision( *p2Missile ) ) {
			wasCollision = true;
		}
		if ( wasCollision ) {
			causeMissileExplosion( *p2Missile );
			deleteMissile( p2Missile );
		}
	}
	// see if missiles collided
	if ( p1Missile != nullptr && p2Missile != nullptr && checkRectRectCollision( *p1Missile, *p2Missile ) ) {
		// blow up first missile, explosion will cause 2nd missile to blow up
		causeMissileExplosion( *p1Missile );
		deleteMissile( p1Missile );
	}

	// check for leech collisions
	for ( LeechAttack& leech : leechAttacks ) {
		if ( leech.target == player2 ) {
			// player 1 launched the attack
			if ( !leech.isAttached && !leech.isReturning ) {
				if ( checkRectRectCollision( leech, *player2 ) ) {
					leech.attachLeech();
				}
			} else if ( leech.isReturning ) {
				if ( checkRectRectCollision( leech, *player1 ) ) {
					leech.isAlive = GL_FALSE;
					addEnergy( P1_SELECTED, leech.amountLeeched );

					// play receive leech sound
					soundEngine->play2D( "resources/sounds/leech_receive.wav", GL_FALSE );
				}
			}
		} else if ( leech.target == player1 ) {
			// player 2 launched the attack
			if ( !leech.isAttached && !leech.isReturning ) {
				if ( checkRectRectCollision( leech, *player1 ) ) {
					leech.attachLeech();
				}
			} else if ( leech.isReturning ) {
				if ( checkRectRectCollision( leech, *player2 ) ) {
					leech.isAlive = GL_FALSE;
					addEnergy( P2_SELECTED, leech.amountLeeched );

					// play receive leech sound
					soundEngine->play2D( "resources/sounds/leech_receive.wav", GL_FALSE );
				}
			}
		}
	}

	// delete dead leaches and ones that are out of bounds
	leechAttacks.erase( std::remove_if( leechAttacks.begin(), leechAttacks.end(), [&] ( const LeechAttack& leech ) {
		return ( ( leech.pos.x < -leech.size.x ) || ( leech.pos.x > width ) || !leech.isAlive );
	} ), leechAttacks.end() );

	// check for grapple collision with target
	if ( p1Grapple != nullptr && !p1Grapple->isAttached && p1Grapple->isAlive ) {
		Collision collision = checkBallRectCollision( *p1Grapple->endSticker, *p1Grapple->target );
		if ( std::get<0>( collision ) ) {
			p1Grapple->attachToTarget();

			// play grapple sound
			soundEngine->play2D( "resources/sounds/grapple_attach.wav", GL_FALSE );
		}
	} else if ( p1Grapple != nullptr && !p1Grapple->isAlive ) {
		// delete grapple
		delete p1Grapple;
		p1Grapple = nullptr;
	}
	if ( p2Grapple != nullptr && !p2Grapple->isAttached && p2Grapple->isAlive ) {
		Collision collision = checkBallRectCollision( *p2Grapple->endSticker, *p2Grapple->target );
		if ( std::get<0>( collision ) ) {
			p2Grapple->attachToTarget();

			// play grapple sound
			soundEngine->play2D( "resources/sounds/grapple_attach.wav", GL_FALSE );
		}
	} else if ( p2Grapple != nullptr && !p2Grapple->isAlive ) {
		// delete grapple
		delete p2Grapple;
		p2Grapple = nullptr;
	}

	// handle powerup collisions
	for ( PowerUp& powerUp : powerUps ) {
		// collisions with walls
		if ( powerUp.object.pos.x <= -powerUp.object.size.x || powerUp.object.pos.x + powerUp.object.size.x >= width ) {
			powerUp.object.isDestroyed = GL_TRUE;
		}
		else if ( powerUp.object.pos.y <= heightRange.x || powerUp.object.pos.y + powerUp.object.size.y >= heightRange.y ) {
			powerUp.object.vel.y = -powerUp.object.vel.y;
		} else if ( std::get<0>( checkBallRectCollision( powerUp.object, *player1 ) ) ) {
			// player 1 got power up
			powerUp.object.isDestroyed = GL_TRUE;
			if (p1PowerUp.type == powerUp.type) {
				p1PowerUp.charges += powerUp.charges;
			}
			else {
				p1PowerUp = powerUp;
			}
		} else if ( std::get<0>( checkBallRectCollision( powerUp.object, *player2 ) ) ) {
			// player 2 got power up
			powerUp.object.isDestroyed = GL_TRUE;
			if (p2PowerUp.type == powerUp.type) {
				p2PowerUp.charges += powerUp.charges;
			}
			else {
				p2PowerUp = powerUp;
			}
		}
	}

	// delete dead leaches and ones that are out of bounds
	powerUps.erase( std::remove_if( powerUps.begin(), powerUps.end(), [] ( const PowerUp& powerUp ) {
		return powerUp.object.isDestroyed ;
	} ), powerUps.end() );

	// handle gravity ball player collisions
	for ( GravityBall& gravBall : gravityBalls ) {

		// only detect collision if not collapsing
		if ( !gravBall.isCollapsing ) {
			playerCollision = checkBallRectCollision( gravBall, *player1 );
			if ( std::get<0>( playerCollision ) ) {
				resolveBallPlayerCollision( gravBall, *player1, 1 );

				// player bounce sound
				soundEngine->play2D( "resources/sounds/tech_bounce.wav", GL_FALSE );

				// set grav ball to selected
				unselectGravBall( P1_SELECTED );
				gravBall.selectedBy = P1_SELECTED;

				// give player energy on bounce
				if ( p1BounceCooldown <= 0.0f ) {
					addEnergy( P1_SELECTED, ENERGY_PER_BOUNCE * ( gravBall.radius / gravBall.MAX_RADIUS ) );
					p1BounceCooldown = BOUNCE_COOLDOWN_TIME;
				}

			} else {
				playerCollision = checkBallRectCollision( gravBall, *player2 );
				if ( std::get<0>( playerCollision ) ) {
					resolveBallPlayerCollision( gravBall, *player2, 2 );

					// player bounce sound
					irrklang::ISound* sound = soundEngine->play2D( "resources/sounds/alien_bounce.wav", GL_FALSE, GL_TRUE, GL_TRUE );
					sound->setVolume( 1.5f );
					sound->setIsPaused( GL_FALSE );

					// set grav ball to selected
					unselectGravBall( P2_SELECTED );
					gravBall.selectedBy = P2_SELECTED;

					// give player energy on bounce
					if ( p2BounceCooldown <= 0.0f ) {
						addEnergy( P2_SELECTED, ENERGY_PER_BOUNCE * ( gravBall.radius / gravBall.MAX_RADIUS ) );
						p2BounceCooldown = BOUNCE_COOLDOWN_TIME;
					}
				}
			}
		}
	}

}

GLboolean GravityPong::checkRectRectCollision( const GameObject& one, const GameObject& two ) const {
	if ( (GLint)one.rotation % 180 == 0 && (GLint)two.rotation % 180 == 0 ) {
		// hancle case where rectangle is not rotated
		// collision with x axis
		bool collisionX = ( one.pos.x + one.size.x >= two.pos.x ) && ( two.pos.x + two.size.x >= one.pos.x );

		// colission with y axis
		bool collisionY = ( one.pos.y + one.size.y >= two.pos.y ) && ( two.pos.y + two.size.y >= one.pos.y );

		return collisionX && collisionY;
	} else {
		// References: http://www.gamedev.net/page/resources/_/technical/game-programming/2d-rotated-rectangle-collision-r2604
		// http://gamedevelopment.tutsplus.com/tutorials/collision-detection-using-the-separating-axis-theorem--gamedev-169
		// http://gamedevelopment.tutsplus.com/tutorials/collision-detection-using-the-separating-axis-theorem--gamedev-169

		// determine if collision occured with rotated rectangles using separation of axis theorem

		// handle case where rectangles are rotated
		glm::vec2 rect1[4];
		glm::vec2 rect2[4];
		one.getVertices( rect1 );
		two.getVertices( rect2 );

		// calculate the 4 axes
		glm::vec2 axes[4] = {
			glm::vec2( glm::normalize( rect1[1] - rect1[0] ) ),
			glm::vec2( glm::normalize( rect1[2] - rect1[1] ) ),
			glm::vec2( glm::normalize( rect2[1] - rect2[0] ) ),
			glm::vec2( glm::normalize( rect2[2] - rect2[1] ) )
		};

		// loop through each axis and project rectangle vertices onto it
		GLfloat min1, max1, min2, max2;
		GLfloat dotProduct;
		for ( int i = 0; i < 4; ++i ) {
			// calculate projections for first rectangle
			min1 = max1 = glm::dot( rect1[0], axes[i] );
			min2 = max2 = glm::dot( rect2[0], axes[i] );
			for ( int v = 1; v < 4; ++v ) {
				dotProduct = glm::dot( rect1[v], axes[i] );
				if ( dotProduct < min1 ) {
					min1 = dotProduct;
				}
				if ( dotProduct > max1 ) {
					max1 = dotProduct;
				}
				dotProduct = glm::dot( rect2[v], axes[i] );
				if ( dotProduct < min2 ) {
					min2 = dotProduct;
				}
				if ( dotProduct > max2 ) {
					max2 = dotProduct;
				}
			}

			// test to see if they are not overlapping
			if ( !( min2 <= max1 && max2 >= min1 ) ) {
				return false;
			}
		}
		return true;
	}
}

GLboolean GravityPong::checkBallBallCollision( const BallObject& one, const BallObject& two ) const {
	return glm::distance( one.pos, two.pos ) <= one.radius + two.radius;
}

Collision GravityPong::checkBallRectCollision( const BallObject& one, const GameObject& two ) const {
	// Reference: http://learnopengl.com/#!In-Practice/2D-Game/Collisions/Collision-detection

	glm::vec2 circlePos = one.pos;
	if ( two.rotation != 0.0f ) {
		// if rectangle is rotated, rotate circle with respect to the rectangles origin center
		GLfloat angle = -two.rotation * ( PI / 180.0f );
		GLfloat unrotatedCircleX = std::cos( angle ) * ( one.getCenter().x - two.getCenter().x ) - std::sin( angle ) * ( one.getCenter().y - two.getCenter().y ) + two.getCenter().x;
		GLfloat unrotatedCircleY = std::sin( angle ) * ( one.getCenter().x - two.getCenter().x ) + std::cos( angle ) * ( one.getCenter().y - two.getCenter().y ) + two.getCenter().y;
		circlePos = glm::vec2( unrotatedCircleX, unrotatedCircleY ) - one.radius;
	}

	// calculate center of ball and rect
	glm::vec2 centerBall( circlePos + one.radius );
	glm::vec2 rectHalfExtents( two.size.x / 2, two.size.y / 2 );
	glm::vec2 centerRect( two.pos.x + rectHalfExtents.x, two.pos.y + rectHalfExtents.y );

	// get direction between ball and rect and clamp it to edge of rectangle
	glm::vec2 direction = centerBall - centerRect;
	glm::vec2 clamped = glm::clamp( direction, -rectHalfExtents, rectHalfExtents );

	// find the closest point of rect and se if it is in circle's radius
	glm::vec2 closest = centerRect + clamped;
	direction = closest - centerBall;

	if ( glm::length( direction ) <= one.radius ) {
		return std::make_tuple( GL_TRUE, vectorDirection( direction ), direction );
	} else {
		return std::make_tuple( GL_FALSE, UP, glm::vec2( 0, 0 ) );
	}
}

GLboolean GravityPong::checkWallsRectCollision( const GameObject& object ) const {
	glm::vec2 vertices[4];
	object.getVertices( vertices );
	// check for wall collisions, handles rotated rectangle
	for ( int i = 0; i < 4; ++i ) {
		if ( vertices[i].x <= 0.0f || vertices[i].x >= width || vertices[i].y <= heightRange.x || vertices[i].y >= heightRange.y ) {
			return true;
		}
	}
	return false;
}

Direction GravityPong::vectorDirection( const glm::vec2 target ) const {
	// Reference: http://learnopengl.com/#!In-Practice/2D-Game/Collisions/Collision-resolution

	glm::vec2 compass[] = {
		glm::vec2( 0.0f, 1.0f ),
		glm::vec2( 1.0f, 0.0f ),
		glm::vec2( 0.0f, -1.0f ),
		glm::vec2( -1.0f, 0.0f )
	};

	GLfloat max = 0.0f;
	Direction bestMatch = UP;
	// uses the dot product to see which of the 4 directions the vector is closest to
	for ( GLuint i = 0; i < 4; ++i ) {
		GLfloat dotProduct = glm::dot( glm::normalize( target ), compass[i] );
		if ( dotProduct > max ) {
			max = dotProduct;
			bestMatch = (Direction)i;
		}
	}
	return bestMatch;
}

void GravityPong::resetGame() {
	// reset ball
	ball->reset( glm::vec2( width / 2.0f - ball->radius, ( heightRange.y + heightRange.x ) / 2.0f - ball->radius ), glm::vec2( 0.0f ) );
	ball->startLaunch();
	currentMaxBallSpeed = START_MAX_BALL_SPEED;
	slowTimeTillLaunch = BALL_LAUNCH_THRESHOLD;

	// reset paddles
	player1->speed = player2->speed = PADDLE_SPEED;
	player2->size = player2->size = PADDLE_SIZE;
	glm::vec2 playerPos = glm::vec2( 0.0f, ( heightRange.y + heightRange.x ) / 2.0f - PADDLE_SIZE.y / 2.0f );
	player1->reset( playerPos );
	playerPos = glm::vec2( width - PADDLE_SIZE.x, ( heightRange.y + heightRange.x ) / 2.0f - PADDLE_SIZE.y / 2.0f );
	player2->reset( playerPos );
	p1Energy = p2Energy = STARTING_ENERGY;
	p1Lives = NUM_LIVES;
	p2Lives = NUM_LIVES;

	// clear gravity balls
	if ( p1ChargingGravBall != nullptr ) {
		delete p1ChargingGravBall;
		p1ChargingGravBall = nullptr;
	}
	if ( p2ChargingGravBall != nullptr ) {
		delete p2ChargingGravBall;
		p2ChargingGravBall = nullptr;
	}
	gravityBalls.clear();

	// clear missile
	if ( p1Missile != nullptr ) {
		delete p1Missile;
		p1Missile = nullptr;
	}
	if ( p2Missile != nullptr ) {
		delete p2Missile;
		p2Missile = nullptr;
	} 

	// clear leeches
	leechAttacks.clear();

	// clear grapples
	if ( p1Grapple != nullptr ) {
		delete p1Grapple;
		p1Grapple = nullptr;
	}
	if ( p2Grapple != nullptr ) {
		delete p2Grapple;
		p2Grapple = nullptr;
	}

	// clear power ups
	powerUps.clear();
	p1PowerUp = PowerUp();
	p2PowerUp = PowerUp();
	nextPowerUpCooldown = MIN_POWERUP_TIME + ( rand() % ( MAX_POWERUP_TIME - MIN_POWERUP_TIME ) );

	// reset punishment
	clearPunishment();

	// stop all sounds
	soundEngine->stopAllSounds();
}

void GravityPong::resolveBallPlayerCollision( BallObject& ball, const PaddleObject player, const int num ) {
	// check where the ball hit the player, and change vel based on location
	GLfloat distance = ball.getCenter().y - player.getCenter().y;
	GLfloat percentage = distance / ( player.size.y / 2 );
	GLfloat angle = ( percentage * MAX_HIT_ANGLE ) * PI / 180.0f;
	GLfloat oldSpeed = glm::length( ball.vel );

	// move accordingly
	ball.vel.x = oldSpeed * std::cos( angle );
	ball.vel.y = oldSpeed * std::sin( angle );

	// if second player reverse the calculated x direction
	if ( num == 2 ) {
		ball.vel.x = -ball.vel.x;
	}
}

void GravityPong::updateGravityBalls( const GLfloat dt ) {
	// update gravity balls
	if ( p1ChargingGravBall != nullptr ) {

		// have any charging gravity ball pull in powerups
		if ( !p1ChargingGravBall->isReversed ) {
			for ( PowerUp& powerUp : powerUps ) {
				p1ChargingGravBall->pullObject( dt, powerUp.object, *soundEngine );
			}
		}

		p1ChargingGravBall->update( dt, heightRange );
		if ( ( !keys[GLFW_KEY_D] && !p1ChargingGravBall->isReversed ) || (!keys[GLFW_KEY_E] && p1ChargingGravBall->isReversed)) {
			unselectGravBall( P1_SELECTED );

			// launch the gravity ball
			p1ChargingGravBall->vel = glm::vec2( p1ChargingGravBall->speed, 0.0f );
			p1ChargingGravBall->selectedBy = P1_SELECTED;
			gravityBalls.push_back( *p1ChargingGravBall );

			delete p1ChargingGravBall;
			p1ChargingGravBall = nullptr;
		}
	}
	if ( p2ChargingGravBall != nullptr ) {

		// have any charging gravity ball pull in powerups
		if ( !p2ChargingGravBall->isReversed ) {
			for ( PowerUp& powerUp : powerUps ) {
				p2ChargingGravBall->pullObject( dt, powerUp.object, *soundEngine );
			}
		}

		p2ChargingGravBall->update( dt, heightRange );
		if ((!keys[GLFW_KEY_DELETE] && !p2ChargingGravBall->isReversed) || (!keys[GLFW_KEY_INSERT] && p2ChargingGravBall->isReversed)) {
			unselectGravBall( P2_SELECTED );

			// launch the gravity ball
			p2ChargingGravBall->vel = glm::vec2( -p2ChargingGravBall->speed, 0.0f );
			p2ChargingGravBall->selectedBy = P2_SELECTED;
			gravityBalls.push_back( *p2ChargingGravBall );

			delete p2ChargingGravBall;
			p2ChargingGravBall = nullptr;
		}
	}
	// pull game ball
	for ( GravityBall& gravBall : gravityBalls ) {
		gravBall.update( dt, heightRange );
		gravBall.pullObject( dt, *ball, *soundEngine );
	}
	// pull missiles
	if ( p1Missile != nullptr ) {
		for ( GravityBall& gravBall : gravityBalls ) {
			gravBall.pullObject( dt, *p1Missile, *soundEngine );
		}
	}
	if ( p2Missile != nullptr ) {
		for ( GravityBall& gravBall : gravityBalls ) {
			gravBall.pullObject( dt, *p2Missile, *soundEngine );
		}
	}
	// pull powerups
	for ( PowerUp& powerUp : powerUps ) {
		for ( GravityBall& gravBall : gravityBalls ) {
			gravBall.pullObject( dt, powerUp.object, *soundEngine );
		}
	}

	// remove gravity balls that are out of bounds
	gravityBalls.erase( std::remove_if( gravityBalls.begin(), gravityBalls.end(), [&] ( const GravityBall& gravBall ) {
		return ( ( gravBall.pos.x < -gravBall.size.x ) || ( gravBall.pos.x > width ) || gravBall.radius <= 0.0f );
	} ), gravityBalls.end() );
}

void GravityPong::renderGUI() const {
	// draw player energy bars
	GLfloat offsetX = ( 0.15f + 0.05 * 0.7f ) * width, offsetY = 0.1f * ( heightRange.x / 3.0f );
	glm::vec2 totalSize = glm::vec2( 0.63f * width, ( heightRange.x / 3.0f ) * 0.7f );
	glm::vec2 pos = glm::vec2( offsetX, offsetY );
	glm::vec2 size = glm::vec2( totalSize.x * ( (GLfloat)p1Energy / ( p1Energy + p2Energy ) ), totalSize.y );

	glm::vec2 pos2 = glm::vec2( pos.x + size.x, offsetY );
	glm::vec2 size2 = glm::vec2( totalSize.x * ( (GLfloat)p2Energy / ( p1Energy + p2Energy ) ), totalSize.y );
	spriteRenderer->drawSprite( ResourceManager::getTexture( "energy" ), pos2, size2, 0.0f, glm::vec4( 0.52f, 0.43f, 0.85f, 1.0f ) );
	spriteRenderer->drawSprite( ResourceManager::getTexture( "energy" ), pos, size, 0.0f, glm::vec4( 0.57f, 0.80f, 0.97f, 1.0f ) );

	// draw gui background
	spriteRenderer->drawSprite( ResourceManager::getTexture( "gui_background" ), glm::vec2( 0.0f ), glm::vec2( width, .15 * height ), 0.0f );

	// draw player energy text
	std::stringstream ss;
	ss << (GLuint)p1Energy;
	textRenderer->renderText( ss.str(), pos.x + width * 0.01f, pos.y + size.y * 0.2f, ( size.y * 0.6f ) / 16.0f, glm::vec3( 0.0f ) );
	ss.str( std::string() );
	ss << (GLuint)p2Energy;
	textRenderer->renderText( ss.str(), pos2.x + size2.x - width * 0.01f, pos.y + size.y * 0.2f, ( size.y * 0.6f ) / 16.0f, glm::vec3( 0.0f ), RIGHT_ALIGNED );

	// draw red punishment marker
	glm::vec2 markerSize = glm::vec2( 4.0f, size.y );
	spriteRenderer->drawSprite( ResourceManager::getTexture( "red" ), glm::vec2( pos.x + 0.25f * totalSize.x - markerSize.x / 2.0f, pos.y ), markerSize, 0.0f, glm::vec4( 0.5f ) );
	spriteRenderer->drawSprite( ResourceManager::getTexture( "red" ), glm::vec2( pos.x + 0.75f * totalSize.x - markerSize.x / 2.0f, pos.y ), markerSize, 0.0f, glm::vec4( 0.5f ) );

	// draw player lives
	glm::vec2 p1Start = glm::vec2( 0.1f * 0.15f * width, 0.245f * 0.1f * heightRange.x );
	glm::vec2 lifeSize = glm::vec2( ( 0.245f * 0.8 * heightRange.x ) / 2.0f, 0.245f * 0.8 * heightRange.x );
	GLfloat xLifeOffset = ( ( 0.8f * 0.15f * width ) - ( lifeSize.x * NUM_LIVES ) ) / ( NUM_LIVES - 1 );
	for ( int i = 0; i < p1Lives; ++i, p1Start.x += ( lifeSize.x + xLifeOffset ) ) {
		spriteRenderer->drawSprite( ResourceManager::getTexture( "life" ), p1Start, lifeSize, 0.0f );
	}
	glm::vec2 p2Start = glm::vec2( width - ( 0.1f * 0.15f * width ) - lifeSize.x, p1Start.y );
	for ( int i = 0; i < p2Lives; ++i, p2Start.x -= ( lifeSize.x + xLifeOffset ) ) {
		spriteRenderer->drawSprite( ResourceManager::getTexture( "life" ), p2Start, lifeSize, 0.0f );
	}

	// draw punishment panels
	glm::vec2 leftPos = glm::vec2( offsetX, heightRange.x / 3.0f + ( heightRange.x * ( 2.0f / 3.0f ) * 0.05f ) );
	glm::vec2 pBoxSize = glm::vec2( ( 0.7f * 0.25f * width ) / 3.0f, 0.9f * heightRange.x * ( 2.0f / 3.0f ) );
	glm::vec2 textBoxSize = glm::vec2( pBoxSize.x * 2.0f, pBoxSize.y );
	glm::vec2 rightPos = glm::vec2( ( width - offsetX ) - pBoxSize.x, heightRange.x / 3.0f + ( heightRange.x * ( 2.0f / 3.0f ) * 0.05f ) );
	glm::vec2 leftTextPos = glm::vec2( leftPos.x + pBoxSize.x + 0.02 * pBoxSize.x, leftPos.y );
	glm::vec2 rightTextPos = glm::vec2( rightPos.x - textBoxSize.x - 0.02 * pBoxSize.x, leftPos.y );
	spriteRenderer->drawSprite( ResourceManager::getTexture( "punishment_text_box" ), leftTextPos, textBoxSize, 0.0f );
	spriteRenderer->drawSprite( ResourceManager::getTexture( "punishment_text_box" ), rightTextPos, textBoxSize, 0.0f );

	// get punishment icon
	Texture image;
	PUNISHMENT_TYPE type;
	if ( nextPunishmentCountdown > 0.0f ) {
		type = nextPunishmentType;
	} else {
		type = punishment.type;
	}
	switch ( type ) {
	case SLOW:
		image = ResourceManager::getTexture( "slow_punishment" );
		break;
	case SHRINK:
		image = ResourceManager::getTexture( "shrink_punishment" );
		break;
	case INVERSE:
		image = ResourceManager::getTexture( "inverse_punishment" );
		break;
	case TRAIL:
		image = ResourceManager::getTexture( "trail_punishment" );
		break;
	case ABUSE:
		image = ResourceManager::getTexture( "abuse_punishment" );
		break;
	case BLIND:
		image = ResourceManager::getTexture( "blind_punishment" );
		break;
	case FLIP:
		image = ResourceManager::getTexture( "flip_punishment" );
		break;
	}

	// draw punishment countdown
	glm::vec2 punishmentPos, punishmentSize;
	GLfloat sizeY = (0.755f * heightRange.x) * 0.9;
	GLfloat scale = (sizeY / pBoxSize.y);
	punishmentSize = pBoxSize * scale;
	if ( nextPunishmentCountdown > 0.0f ) {

		if ( state != GAME_OVER && state != GAME_PAUSED ) {
			// draw punishment countdown
			ss.str( std::string() );
			ss << (GLint)nextPunishmentCountdown;
			textRenderer->renderText( ss.str(), width / 2.0f, heightRange.x * ( 1.0f / 3.0f ) + heightRange.x * ( 2.0f / 3.0f ) * 0.3f, ( heightRange.x * ( 2.0f / 3.0f ) * 0.4f ) / 16.0f, glm::vec3( 1.0f ), CENTERED );
		}

		// draw punishment icon and text
		GLfloat p1EnergyRatio = p1Energy / ( p1Energy + p2Energy );
		if ( p1EnergyRatio <= 0.25f ) {
			spriteRenderer->drawSprite( image, leftPos, pBoxSize, 0.0f, glm::vec4( 1.0f ), GL_TRUE );
			spriteRenderer->drawSprite( ResourceManager::getTexture( "black_punishment_box" ), rightPos, pBoxSize, 0.0f );
			textRenderer->renderText( Punishment::getPunishmentName( type ), leftTextPos.x + textBoxSize.x / 2.0f, leftTextPos.y + textBoxSize.y * 0.35f, ( textBoxSize.y * 0.3f ) / 16.0f, glm::vec3( 0.0f ), CENTERED );
		} else if ( p1EnergyRatio >= 0.75f ) {
			spriteRenderer->drawSprite( ResourceManager::getTexture( "black_punishment_box" ), leftPos, pBoxSize, 0.0f );
			spriteRenderer->drawSprite( image, rightPos, pBoxSize, 0.0f );
			textRenderer->renderText( Punishment::getPunishmentName( type ), rightTextPos.x + textBoxSize.x / 2.0f, rightTextPos.y + textBoxSize.y * 0.35, ( textBoxSize.y * 0.3f ) / 16.0f, glm::vec3( 0.0f ), CENTERED );
		} else {
			spriteRenderer->drawSprite( image, leftPos, pBoxSize, 0.0f, glm::vec4( 1.0f ), GL_TRUE );
			spriteRenderer->drawSprite( image, rightPos, pBoxSize, 0.0f );
			textRenderer->renderText( Punishment::getPunishmentName( type ), leftTextPos.x + textBoxSize.x / 2.0f, leftTextPos.y + textBoxSize.y * 0.35f, ( textBoxSize.y * 0.3f ) / 16.0f, glm::vec3( 0.0f ), CENTERED );
			textRenderer->renderText( Punishment::getPunishmentName( type ), rightTextPos.x + textBoxSize.x / 2.0f, rightTextPos.y + textBoxSize.y * 0.35, ( textBoxSize.y * 0.3f ) / 16.0f, glm::vec3( 0.0f ), CENTERED );
		}
	} else {
		ss.str( std::string() );
		if ( punishment.charges > 0 ) {
			ss << "C:" << punishment.charges;
		} else {
			ss << "T:" << (GLint)punishment.timeLeft;
		}

		spriteRenderer->drawSprite( ResourceManager::getTexture( "black_punishment_box" ), rightPos, pBoxSize, 0.0f );
		spriteRenderer->drawSprite( ResourceManager::getTexture( "black_punishment_box" ), leftPos, pBoxSize, 0.0f );

		// draw active punishment
		if ( punishment.player == P1_SELECTED ) {
			punishmentPos = glm::vec2( 0.02f * 0.15f * width + ( 1.5 * punishmentSize.x ), ( 0.245f * heightRange.x ) * 1.1f);
			// draw current punishment and its duration
			textRenderer->renderText( Punishment::getPunishmentName( type ), leftTextPos.x + textBoxSize.x / 2.0f, leftTextPos.y + textBoxSize.y * 0.35f, ( textBoxSize.y * 0.3f ) / 16.0f, glm::vec3( 0.0f ), CENTERED );
			textRenderer->renderText( ss.str(), leftPos.x + pBoxSize.x / 2.0f, leftPos.y + pBoxSize.y * 0.35f, ( pBoxSize.y * 0.3f ) / 16.0f, glm::vec3( 1.0f ), CENTERED );
		} else {
			punishmentPos = glm::vec2( width - ( 0.02f * 0.15f * width ) - ( 2.5 * punishmentSize.x ), ( 0.245f * heightRange.x ) * 1.1f );
			// draw current punishment and its duration
			textRenderer->renderText( Punishment::getPunishmentName( type ), rightTextPos.x + textBoxSize.x / 2.0f, rightTextPos.y + textBoxSize.y * 0.35, ( textBoxSize.y * 0.3f ) / 16.0f, glm::vec3( 0.0f ), CENTERED );
			textRenderer->renderText( ss.str(), rightPos.x + pBoxSize.x / 2.0f, leftPos.y + pBoxSize.y * 0.35f, ( pBoxSize.y * 0.3f ) / 16.0f, glm::vec3( 1.0f ), CENTERED );
		}
		spriteRenderer->drawSprite( image, punishmentPos, punishmentSize, 0.0f, glm::vec4( 1.0f ), punishment.player == P1_SELECTED );
	}

	// render powerups
	if (p1PowerUp.charges > 0) {
		glm::vec2 pos = glm::vec2(0.02f * 0.15f * width, (0.245f * heightRange.x) * 1.1f);
		spriteRenderer->drawSprite(p1PowerUp.object.sprite, pos, punishmentSize, 0.0f, glm::vec4(1.0f));
		ss.str(std::string());
		ss << p1PowerUp.charges;
		glm::vec2 textPos = glm::vec2( punishmentSize.x / 10.0f, pos.y + punishmentSize.y * 0.7f);
		textRenderer->renderText(ss.str(), textPos.x, textPos.y, (punishmentSize.y * 0.2f) / 16.0f, glm::vec3(1.0f));
	}
	if (p2PowerUp.charges > 0) {
		glm::vec2 pos = glm::vec2(width - (0.02f * 0.15f * width) - punishmentSize.x, (0.245f * heightRange.x) * 1.1f);
		spriteRenderer->drawSprite(p2PowerUp.object.sprite, pos, punishmentSize, 0.0f, glm::vec4(1.0f), GL_FALSE);
		ss.str(std::string());
		ss << p2PowerUp.charges;
		glm::vec2 textPos = glm::vec2(width - punishmentSize.x / 10.0f, pos.y + punishmentSize.y * 0.7f);
		textRenderer->renderText(ss.str(), textPos.x, textPos.y, (punishmentSize.y * 0.2f) / 16.0f, glm::vec3(1.0f), RIGHT_ALIGNED);
	}

	if ( state == GAME_OVER ) {
		textRenderer->renderText( "Press ENTER to start or ESC to quit", 0.5f * width,
			heightRange.x * ( 1.0f / 3.0f ) + heightRange.x * ( 2.0f / 3.0f ) * 0.43f, ( heightRange.x * ( 2.0f / 3.0f ) * 0.14f ) / 16.0f, glm::vec3( 1.0f ), CENTERED );
	} else if ( state == GAME_PAUSED ) {
		textRenderer->renderText( "Game Paused : P to Unpause", 0.5f * width,
			heightRange.x * ( 1.0f / 3.0f ) + heightRange.x * ( 2.0f / 3.0f ) * 0.43f, ( heightRange.x * ( 2.0f / 3.0f ) * 0.14f ) / 16.0f, glm::vec3( 1.0f ), CENTERED );
	}

}

void GravityPong::renderGUIRetro() const {
	// draw gui border
	retroRenderer.renderLine( glm::vec2( 0.0f, 0.97f * heightRange.x ), glm::vec2( width, 0.97f * heightRange.x ) );

	// draw player energy bars
	GLfloat offsetX = 0.10f, offsetY = 0.02f;
	glm::vec2 pos = glm::vec2( offsetX * width, offsetY * height );
	glm::vec2 size = glm::vec2( ( width * ( 1.0f - 2.0f * offsetX ) ) * ( (GLfloat)p1Energy / ( p1Energy + p2Energy ) ), height * 0.03f );
	retroRenderer.renderRect( pos, pos + size );

	glm::vec2 pos2 = glm::vec2( pos.x + size.x, offsetY * height );
	glm::vec2 size2 = glm::vec2( ( width * ( 1.0f - 2.0f * offsetX ) ) * ( (GLfloat)p2Energy / ( p1Energy + p2Energy ) ), height * 0.03f );
	retroRenderer.renderRect( pos2, pos2 + size2 );

	// draw player lives in tally markers
	GLfloat startX = pos.x * 0.25f, endX = pos.x * 0.75;
	GLfloat distBetween = ( endX - startX ) / NUM_LIVES;
	for ( int i = 0; i < p1Lives; ++i, startX += distBetween ) {
		retroRenderer.renderLine( glm::vec2( startX, pos.y ), glm::vec2( startX, pos.y + size.y ) );
	}

	startX = width - ( pos.x * 0.75f );
	endX = width - ( pos.x * 0.25f );
	distBetween = ( endX - startX ) / NUM_LIVES;
	for ( int i = p2Lives; i > 0; --i, endX -= distBetween ) {
		retroRenderer.renderLine( glm::vec2( endX, pos.y ), glm::vec2( endX, pos.y + size.y ) );
	}

	// draw punishment notification
	offsetX = 0.40f;
	pos = glm::vec2( offsetX * width, pos.y + size.y + heightRange.x * 0.03f );
	size = glm::vec2( ( width * ( 1.0f - 2.0f * offsetX ) ), 0.5f * heightRange.x );
	//retroRenderer.renderRect( pos, pos + size );

	if ( nextPunishmentCountdown > 0.0f ) {
		pos = glm::vec2( pos.x + size.x * 0.05f, pos.y + size.y * 0.3f );
		// draw punishment countdown
		retroRenderer.renderRect( glm::vec2( pos ), glm::vec2( pos.x + size.x * 0.9f * ( nextPunishmentCountdown / PUNISHMENT_COUNTDOWN ), pos.y + size.y * 0.6f ) );
	}

	if ( state == GAME_OVER ) {
		retroRenderer.renderEnter( glm::vec2( width / 2.0f, ( heightRange.y + heightRange.x ) / 3.0f ), glm::vec2( width / 5.0f, ( heightRange.y + heightRange.x ) / 10.0f ) );
	}
}

void GravityPong::unselectGravBall( const PLAYER_SELECTED player ) {
	if ( player == NO_ONE ) {
		return;
	}

	GravityBall* gravBallPtr = findSelectedGravBall( player );
	if ( gravBallPtr != nullptr ) {
		gravBallPtr->selectedBy = NO_ONE;
	}
}

GravityBall* GravityPong::findSelectedGravBall( const PLAYER_SELECTED player ) {
	if ( player == NO_ONE ) {
		return nullptr;
	}

	for ( GravityBall& gravBall : gravityBalls ) {
		if ( gravBall.selectedBy == player ) {
			return &gravBall;
		}
	}
}

void GravityPong::handleCooldowns( const GLfloat dt ) {
	// reduce cooldowns
	if ( p1BounceCooldown > 0.0f ) {
		p1BounceCooldown -= dt;
	}
	if ( p2BounceCooldown > 0.0f ) {
		p2BounceCooldown -= dt;
	}
	if ( p1MissileCooldown > 0.0f ) {
		p1MissileCooldown -= dt;
	}
	if ( p2MissileCooldown > 0.0f ) {
		p2MissileCooldown -= dt;
	}
	if ( nextPowerUpCooldown > 0.0f && powerUps.size() < MAX_NUM_POWERUPS ) {
		nextPowerUpCooldown -= dt;
		// apply punishment if cooldown reaches 0
		if ( nextPowerUpCooldown <= 0.0f ) {
			nextPowerUpCooldown = MIN_POWERUP_TIME + ( rand() % ( MAX_POWERUP_TIME - MIN_POWERUP_TIME ) );
			POWERUP_TYPE randomPower = (POWERUP_TYPE)( rand() % NUM_POWERUPS );
			glm::vec2 pos( width / 2.0f - POWERUP_RADIUS, heightRange.x + ( rand() % (int)( ( heightRange.y - heightRange.x - POWERUP_RADIUS * 2.0f) ) ) );
			powerUps.push_back( PowerUp( randomPower, POWERUP_RADIUS, pos ) );
		}
	}
	if ( nextPunishmentCountdown > 0.0f && punishment.player == NO_ONE ) {
		nextPunishmentCountdown -= dt;
		// apply punishment if cooldown reaches 0
		if ( nextPunishmentCountdown <= 0.0f ) {
			dealPunishment();
		}
	} else if ( punishment.player != NO_ONE && punishment.timeLeft > 0.0f || punishment.charges > 0 ) {
		if ( punishment.timeLeft > 0.0f ) {
			punishment.timeLeft -= dt;

			if ( punishment.type == TRAIL && punishment.charges >= (int)punishment.timeLeft + 1 ) {
				GravityBall gravBall( punishment.paddle->getCenter() - GRAV_STARTING_RADIUS, GRAV_STARTING_RADIUS, ResourceManager::getTexture( "gravity_ball" ), 0.0f, GRAV_STARTING_RADIUS, 100.0f );
				gravBall.isCollapsing = GL_TRUE;
				gravBall.color = glm::vec4( 1.0f );
				gravityBalls.push_back( gravBall );
				punishment.charges--;
			}

			// undo punishment
			if ( punishment.timeLeft <= 0.0f ) {
				clearPunishment();
			}
		} else {

			if ( !ball->isLaunching ) {
				--punishment.charges;
				ball->startLaunch( punishment.player );
			}


			if ( punishment.charges == 0 ) {
				switch ( punishment.type ) {
				case ABUSE:
					punishment.timeLeft = PRE_LAUNCH_TIME;
					break;
				}
			}
		}
	}
}

void GravityPong::dealPunishment() {
	// select player to punish based on energy Levels
	PLAYER_SELECTED selectedPlayer = NO_ONE;
	PaddleObject* paddle = nullptr;
	GLfloat p1EnergyPercent = p1Energy / ( p1Energy + p2Energy );

	// select player to punish based on energy levels
	if ( p1EnergyPercent <= 0.25f ) {
		selectedPlayer = P1_SELECTED;
		paddle = player1;
	} else if ( p1EnergyPercent >= 0.75f ) {
		selectedPlayer = P2_SELECTED;
		paddle = player2;
	} else {
		int randomPlayer = rand() % 100;
		if ( randomPlayer <= 100 * ( 1.0 - p1EnergyPercent ) ) {
			selectedPlayer = P1_SELECTED;
			paddle = player1;
		} else {
			selectedPlayer = P2_SELECTED;
			paddle = player2;
		}
	}

	punishment = Punishment( selectedPlayer, nextPunishmentType, paddle );
	switch ( nextPunishmentType ) {
	case SLOW:
		punishment.paddle->speed = PADDLE_SPEED / 1.5f;
		break;
	case SHRINK:
		paddle->pos.y += 0.25f * PADDLE_SIZE.y;
		punishment.paddle->size.y = PADDLE_SIZE.y * SHRINK_AMOUNT;
		break;
	case ABUSE:
		ball->startLaunch( punishment.player );
		--punishment.charges;
		break;
	case INVERSE:
		punishment.paddle->speed = -punishment.paddle->speed;
		break;
	case BLIND:
		if ( punishment.paddle == player1 ) {
			postEffectsRenderer->blindPlayer( *player1, BLIND_RANGE, heightRange );
		} else {
			postEffectsRenderer->blindPlayer( *player2, BLIND_RANGE, heightRange );
		}
		break;
	case FLIP:
		if ( punishment.paddle == player1 ) {
			postEffectsRenderer->flipScreen( glm::vec2( 0.0f, width / 3.0f ), heightRange );
		} else {
			postEffectsRenderer->flipScreen( glm::vec2( width / 3.0f * 2.0f, width ), heightRange );
		}
		break;
	}

	// player punishment sound
	if ( punishment.player == P1_SELECTED ) {
		soundEngine->play2D( "resources/sounds/tech_punished.wav", GL_FALSE );
	} else {
		soundEngine->play2D( "resources/sounds/alien_punished.wav", GL_FALSE );
	}
}

void GravityPong::clearPunishment() {
	if ( punishment.player != NO_ONE ) {
		switch ( punishment.type ) {
		case SLOW:
		case INVERSE:
			punishment.paddle->speed = PADDLE_SPEED;
			break;
		case SHRINK:
			punishment.paddle->pos.y -= SHRINK_AMOUNT / 2.0f * PADDLE_SIZE.y;
			punishment.paddle->size.y = PADDLE_SIZE.y;
			break;
		case BLIND:
		case FLIP:
			postEffectsRenderer->clearEffects();
			break;
		}

		punishment = Punishment();
	}
	// randomly selected the next punishment
	nextPunishmentCountdown = PUNISHMENT_COUNTDOWN;
	int randomPunishment = rand() % NUM_PUNISHMENTS;
	nextPunishmentType = (PUNISHMENT_TYPE)randomPunishment;
}

void GravityPong::causeMissileExplosion( const Missile& missile, const GLboolean missileCheck ) {
	// add explosion to the game
	std::string textureName = &missile == p1Missile ? "tech_explosion" : "alien_explosion";
	explosions.push_back( Explosion( missile.getCenter() - EXPLOSION_RADIUS, EXPLOSION_RADIUS, ResourceManager::getTexture( textureName ), EXPLOSION_TIME ) );

	// play explosion sound
	irrklang::ISound* sound = soundEngine->play2D( "resources/sounds/explosion_sound.flac", GL_FALSE, GL_TRUE );
	sound->setVolume( 0.75f );
	sound->setIsPaused( GL_FALSE );

	// check to see if ball was in the explosion
	GLfloat ballDist = glm::distance( ball->getCenter(), missile.getCenter() );
	if ( ballDist <= EXPLOSION_RADIUS ) {
		// launch the ball
		glm::vec2 ballDir = glm::normalize( ball->getCenter() - missile.getCenter() );
		ball->vel += MISSILE_POWER * ( 0.5f + ( 0.5f - ( ballDist / EXPLOSION_RADIUS ) ) ) * ballDir;
	}

	// check to see if gravity balls were hit in the explosion
	for ( GravityBall& gravBall : gravityBalls ) {
		if ( glm::distance( gravBall.getCenter(), missile.getCenter() ) <= gravBall.radius + EXPLOSION_RADIUS ) {
			gravBall.radius = 0.0f;
		}
	}

	// check to see if paddles were hit in the explosion
	Collision collision = checkBallRectCollision( explosions.back(), *player1 );
	if ( std::get<0>( collision ) ) {
		player1->stunnedTimer = EXPLOSION_STUN_TIME;
	} else {
		collision = checkBallRectCollision( explosions.back(), *player2 );
		if ( std::get<0>( collision ) ) {
			player2->stunnedTimer = EXPLOSION_STUN_TIME;
		}
	}

	if ( missileCheck ) {
		// check to see if other missile was hit in the explosion
		if ( p1Missile == &missile && p2Missile != nullptr ) {
			// check to see if p2 missile was hit
			collision = checkBallRectCollision( explosions.back(), *p2Missile );
			if ( std::get<0>( collision ) ) {
				causeMissileExplosion( *p2Missile, GL_FALSE );
				deleteMissile( p2Missile );
			}
		} else if ( p2Missile == &missile && p1Missile != nullptr ) {
			// check to see if p1 missile was hit
			collision = checkBallRectCollision( explosions.back(), *p1Missile );
			if ( std::get<0>( collision ) ) {
				causeMissileExplosion( *p1Missile, GL_FALSE );
				deleteMissile( p1Missile );
			}
		}
	}
}

void GravityPong::deleteMissile( Missile*& missile ) {
	if ( missile == p1Missile ) {
		p1MissileCooldown = MISSILE_COOLDOWN;
	} else if ( missile == p2Missile ) {
		p2MissileCooldown = MISSILE_COOLDOWN;
	}
	delete missile;
	missile = nullptr;
}

void GravityPong::addEnergy( PLAYER_SELECTED player, GLfloat energy ) {

	const PaddleObject* plr;
	if ( player == P1_SELECTED ) {
		plr = player1;
	} else if ( player == P2_SELECTED ) {
		plr = player2;
	} else {
		return;
	}

	// check to see if leach attached to player 1
	for ( int i = 0; i < leechAttacks.size() && energy > 0; ++i ) {
		LeechAttack& leech = leechAttacks[i];
		if ( leech.target == plr && leech.isAttached ) {
			// give energy to leech and any left overs to the player
			energy = leech.addEnergy( energy, *soundEngine );
		}
	}

	if ( player == P1_SELECTED ) {
		p1Energy += energy;
	} else if ( player == P2_SELECTED ) {
		p2Energy += energy;
	}
}