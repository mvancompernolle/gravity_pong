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
	BLIND,
	FLIP,
	NUM_PUNISHMENTS
};

enum POWERUP_TYPE {
	MISSILE,
	LEECH,
	GRAPPLE,
	NUM_POWERUPS,
	NONE
};

typedef std::tuple<GLboolean, Direction, glm::vec2> Collision;

struct PowerUp {
	BallObject object;
	POWERUP_TYPE type;
	GLuint charges;

	PowerUp() {
		type = NONE;
		charges = 0;
	}

	PowerUp(POWERUP_TYPE type, GLfloat radius, glm::vec2 pos) {
		this->type = type;
		switch (type) {
		case MISSILE:
			object = BallObject( pos, radius, ResourceManager::getTexture( "missile_powerup" ) );
			charges = 3;
			break;
		case LEECH:
			object = BallObject( pos, radius, ResourceManager::getTexture( "leech_powerup" ) );
			charges = 3;
			break;
		case GRAPPLE:
			object = BallObject( pos, radius, ResourceManager::getTexture( "grapple_powerup" ) );
			charges = 2;
			break;
		}
		object.mass = 5.0f;
		object.color = glm::vec4(glm::vec3(1.0f), 0.5f);
	}

	void update( GLfloat dt ) {
		object.update( dt );
	}

	void draw( SpriteRenderer& renderer ) {
		object.draw( renderer );
	}
};

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

	Punishment(PLAYER_SELECTED player, PUNISHMENT_TYPE type, PaddleObject* paddle) {
		this->type = type;
		this->player = player;
		this->paddle = paddle;

		switch (type) {
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
			timeLeft = 5.0f;
			charges = 5;
			break;
		case BLIND:
			timeLeft = 10.0f;
			charges = 0;
			break;
		case FLIP:
			timeLeft = 10.0f;
			charges = 0;
			break;
		}
	}

	static std::string getPunishmentName(PUNISHMENT_TYPE type) {
		switch (type) {
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
		case BLIND:
			return "Blind";
			break;
		case FLIP:
			return "Flip";
			break;
		}
	}

};

class GravityPong : public Game {
public:
	GravityPong(GLuint width, GLuint height);
	~GravityPong();
	void	init();
	void	processInput(const GLfloat dt);
	void	update(const GLfloat dt);
	void	render();

private:
	// GUI
	const GLfloat GUI_PERCENT = 0.15f;
	const GLfloat GAME_VOLUME = 0.5f;

	// ball constants
	const GLfloat DEFAULT_BALL_SPEED = 1000.0f;
	const GLfloat MAX_HIT_ANGLE = 70.0f;
	const GLfloat MIN_BALL_SPEED_X = 200.0f;
	const GLfloat END_MAX_BALL_SPEED = 1400.0f;
	const GLfloat START_MAX_BALL_SPEED = 1000.0f;
	const GLfloat BALL_MAX_SPEED_INCREASE_RATE = 10;
	const GLfloat BALL_LAUNCH_THRESHOLD = 0.5f;

	// gravity  ball
	const GLfloat GRAV_STARTING_RADIUS = 20.0f;
	const GLuint GRAV_BALL_COST = 100;

	// missile
	const GLuint MISSILE_COOLDOWN = 1.0f;
	const GLuint MISSILE_POWER = 2000;
	const GLfloat EXPLOSION_TIME = 0.5f;
	const GLfloat EXPLOSION_RADIUS = 200.0f;
	const GLfloat EXPLOSION_STUN_TIME = 1.0f;
	const glm::vec2 MISSILE_SIZE;

	// leech
	const glm::vec2 LEECH_SIZE = glm::vec2(40.0f, 20.0f);
	const GLfloat LEECH_SPEED;

	// grapple
	const GLfloat GRAPPLE_ANCHOR_RADIUS = 20.0f;
	const GLfloat GRAPPLE_LINK_WIDTH = 10.0f;
	const GLfloat GRAPPLE_SPEED = 2000.0f;
	const GLfloat GRAPPLE_DURATION = 3.0f;
	const GLfloat GRAPPLE_PULL_STRENGTH = 10.0f;

	// player
	const GLuint ENERGY_PER_SECOND = 10;
	const GLuint NUM_LIVES = 5;
	const GLuint ENERGY_PER_BOUNCE = 50;
	const GLfloat BOUNCE_COOLDOWN_TIME = 0.25f;
	const GLfloat PADDLE_SPEED;
	const GLfloat STARTING_ENERGY = 500.0f;
	const glm::vec2 PADDLE_SIZE;

	// punishments
	const GLfloat PUNISHMENT_COUNTDOWN = 10.0f;
	const GLfloat SHRINK_AMOUNT = 0.65;
	const GLfloat BLIND_RANGE = 400.0f;

	// power ups
	const GLuint		MIN_POWERUP_TIME = 10;
	const GLuint		MAX_POWERUP_TIME = 15;
	const GLuint		MAX_NUM_POWERUPS = 2;
	const GLfloat		POWERUP_RADIUS;

	GLboolean					inRetroMode;
	GameState					state;
	SpriteRenderer*				spriteRenderer;
	RetroRenderer				retroRenderer;
	PostProcessor*				postEffectsRenderer;
	ParticleGenerator*			particlesRenderer;
	TextRenderer*				textRenderer;
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
	GLfloat						nextPunishmentCountdown, nextPowerUpCooldown;
	PUNISHMENT_TYPE				nextPunishmentType;
	Punishment					punishment;
	std::list<PowerUp>			powerUps;
	PowerUp						p1PowerUp, p2PowerUp;
	GLfloat						slowTimeTillLaunch, currentMaxBallSpeed;

	// sounds
	irrklang::ISoundEngine*		soundEngine;

	void						renderNormal();
	void						renderRetro();
	void						renderGUIRetro() const;
	void						renderGUI() const;
	GLboolean					checkRectRectCollision(const GameObject& one, const GameObject& two) const;
	GLboolean					checkBallBallCollision(const BallObject& one, const BallObject& two) const;
	Collision					checkBallRectCollision(const BallObject& one, const GameObject& two) const;
	GLboolean					checkWallsRectCollision(const GameObject& object) const;
	Direction					vectorDirection(const glm::vec2 target) const;
	void						handleCollisions();
	void						resolveBallPlayerCollision(BallObject& ball, const PaddleObject player, const int num);
	void						resetGame();
	void						updateGravityBalls(const GLfloat dt);
	void						unselectGravBall(const PLAYER_SELECTED player);
	GravityBall*				findSelectedGravBall(const PLAYER_SELECTED player);
	void						handleCooldowns(const GLfloat dt);
	void						dealPunishment();
	void						clearPunishment();
	void						causeMissileExplosion(const Missile& missile, const GLboolean missileCheck = GL_TRUE);
	void						deleteMissile(Missile*& missile);
	void						addEnergy(PLAYER_SELECTED player, GLfloat energy);
};

#endif // GRAVITY_PONG_H