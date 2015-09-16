#pragma once

#ifndef GRAVITY_PONG_H
#define GRAVITY_PONG_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <irrklang/irrKlang.h>

#include <list>

#include "Game.h"
#include "SpriteRenderer.h"
#include "RetroRenderer.h"
#include "PostProcessor.h"
#include "ParticleGenerator.h"
#include "TextRenderer.h"
#include "ResourceManager.h"
#include "PaddleObject.h"
#include "GameBall.h"
#include "GravityBall.h"
#include "Missile.h"
#include "Explosion.h"
#include "player_selected.h"
#include "LeechAttack.h"
#include "GrappleAttack.h"

enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_OVER,
	GAME_PAUSED
};

enum Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

enum PUNISHMENT_TYPE {
	SLOW, 
	SHRINK,
	ABUSE, 
	INVERSE,
	TRAIL,
	NUM_PUNISHMENTS
};

typedef std::tuple<GLboolean, Direction, glm::vec2> Collision;

struct Punishment {
	PLAYER_SELECTED player;
	PUNISHMENT_TYPE type;
	GLfloat			timeLeft;
	GLuint			charges;
	PaddleObject*	paddle;

	Punishment() {
		player = NO_ONE;
		type = SLOW;
		timeLeft = 0.0f;
		charges = 0;
		paddle = nullptr;
	}

	Punishment( PLAYER_SELECTED player, PUNISHMENT_TYPE type, PaddleObject* paddle ) {
		this->type = type;
		this->player = player;
		this->paddle = paddle;

		switch( type ) {
		case SLOW:
			timeLeft = 10.0f;
			charges = 0;
			break;
		case SHRINK:
			timeLeft = 10.0f;
			charges = 0;
			break;
		case ABUSE:
			timeLeft = 0.0f;
			charges = 3;
			break;
		case INVERSE:
			timeLeft = 5.0f;
			charges = 0;
			break;
		case TRAIL:
			timeLeft = 7.0f;
			charges = 7;
			break;
		}
	}

	static std::string getPunishmentName( PUNISHMENT_TYPE type ) {
		switch( type ) {
		case SLOW:
			return "Slow";
			break;
		case SHRINK:
			return "Shrink";
			break;
		case ABUSE:
			return "Abuse";
			break;
		case INVERSE:
			return "Inverse";
			break;
		case TRAIL:
			return "Trail";
			break;
		}
	}

};

class GravityPong : public Game {
public:
	// GUI
	const GLfloat GUI_PERCENT = 0.15f;

	// ball constants
	const GLfloat DEFAULT_BALL_SPEED = 1000.0f;
	const GLfloat MAX_HIT_ANGLE = 70.0f;
	//const GLfloat MIN_BALL_SPEED = 300.0f;
	const GLfloat MIN_BALL_SPEED_X = 300.0f;
	const GLfloat MAX_BALL_SPEED = 1200.0f;

	// gravity  ball
	const GLfloat GRAV_STARTING_RADIUS = 20.0f;
	const GLuint GRAV_BALL_COST = 200;

	// missile
	const GLuint MISSILE_COOLDOWN = 1.0f;
	const GLuint MISSILE_COST = 300;
	const GLuint MISSILE_POWER = 2000;
	const GLfloat EXPLOSION_TIME = 0.5f;
	const GLfloat EXPLOSION_RADIUS = 150.0f;
	const GLfloat EXPLOSION_STUN_TIME = 1.0f;
	const glm::vec2 MISSILE_SIZE;

	// leech
	const GLfloat LEECH_RADIUS = 20.0f;
	const GLfloat LEECH_SPEED;
	const GLuint LEECH_COST = 200.0f;

	// grapple
	const GLfloat GRAPPLE_ANCHOR_RADIUS = 15.0f;
	const GLfloat GRAPPLE_LINK_WIDTH = 10.0f;
	const GLfloat GRAPPLE_SPEED = 1250.0f;
	const GLfloat GRAPPLE_COST = 500.0f;
	const GLfloat GRAPPLE_DURATION = 5.0f;
	const GLfloat GRAPPLE_PULL_STRENGTH = 10.0f;

	// player
	const GLuint ENERGY_PER_SECOND = 20;
	const GLuint NUM_LIVES = 5;
	const GLuint ENERGY_PER_BOUNCE = 100;
	const GLfloat BOUNCE_COOLDOWN_TIME = 0.25f;
	const GLfloat PADDLE_SPEED;
	const glm::vec2 PADDLE_SIZE;

	// punishments
	const GLfloat PUNISHMENT_COUNTDOWN = 7.0f;
	const GLfloat SHRINK_AMOUNT = 0.65;

			GravityPong( GLuint width, GLuint height );
			~GravityPong();
	void	init();
	void	processInput( const GLfloat dt );
	void	update( const GLfloat dt );
	void	render();

private:
	GLboolean					inRetroMode;
	GameState					state;
	SpriteRenderer*				spriteRenderer;
	RetroRenderer				retroRenderer;
	PostProcessor*				postEffectsRenderer;
	ParticleGenerator*			particlesRenderer;
	TextRenderer*				textRenderer;
	irrklang::ISoundEngine*		soundEngine;
	GLuint						p1Lives, p2Lives;
	PaddleObject				*player1, *player2;
	GameBall*					ball;
	std::list<GravityBall>		gravityBalls;
	std::list<Explosion>		explosions;
	GravityBall					*p1ChargingGravBall, *p2ChargingGravBall;
	Missile						*p1Missile, *p2Missile;
	GrappleAttack				*p1Grapple, *p2Grapple;
	std::vector<LeechAttack>	leechAttacks;
	GLfloat						p1MissileCooldown, p2MissileCooldown;
	GLfloat						p1Energy, p2Energy;
	glm::vec2					heightRange;
	GLfloat						p1BounceCooldown, p2BounceCooldown;
	GLfloat						nextPunishmentCountdown;
	PUNISHMENT_TYPE						nextPunishmentType;
	Punishment					punishment;
	GLboolean					p1IsGravReversed, p2IsGravReversed;

	void						renderNormal( );
	void						renderRetro( );
	void						renderGUIRetro() const;
	void						renderGUI() const;
	GLboolean					checkRectRectCollision( const GameObject& one, const GameObject& two ) const;
	GLboolean					checkBallBallCollision( const BallObject& one, const BallObject& two ) const;
	Collision					checkBallRectCollision( const BallObject& one, const GameObject& two ) const;
	GLboolean					checkWallsRectCollision( const GameObject& object ) const;
	Direction					vectorDirection( const glm::vec2 target ) const;
	void						handleCollisions();
	void						resolveBallPlayerCollision( BallObject& ball, const PaddleObject player, const int num );
	void						resetGame();
	void						updateGravityBalls( const GLfloat dt );
	void						unselectGravBall( const PLAYER_SELECTED player );
	GravityBall*				findSelectedGravBall( const PLAYER_SELECTED player );
	void						handleCooldowns( const GLfloat dt );
	void						dealPunishment();
	void						clearPunishment();
	void						rotateRectangle( glm::vec2 rect[4], const GLfloat rotation, const glm::vec2 center = glm::vec2( 0.0f ) ) const;
	void						causeMissileExplosion( const Missile& missile, const GLboolean missileCheck = GL_TRUE );
	void						deleteMissile( Missile*& missile );
	void						addEnergy( PLAYER_SELECTED player, GLfloat energy );
};

#endif // GRAVITY_PONG_H