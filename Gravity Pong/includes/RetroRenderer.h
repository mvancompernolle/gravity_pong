#pragma once

#ifndef RETRO_RENDERER_H
#define	RETRO_RENDERER_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "GameObject.h"
#include "GameBall.h"
#include "PaddleObject.h"
#include "GravityBall.h"
#include "Missile.h"
#include "LeechAttack.h"
#include "GrappleAttack.h"

class RetroRenderer {
public:
			RetroRenderer();
			~RetroRenderer();

	void	renderCircle( glm::vec2 center, GLfloat radius, GLuint numLines ) const;
	void	renderRect( glm::vec2 topLeft, glm::vec2 bottomRight ) const;
	void	renderTriangle( glm::vec2 p1, glm::vec2 p2, glm::vec2 p3 ) const;
	void	renderLine( glm::vec2 v1, glm::vec2 v2 ) const;
	void	renderGameBall( const GameBall& ball ) const;
	void	renderPaddle( const PaddleObject& paddle ) const;
	void	renderGravityBall( const GravityBall& gravBall ) const;
	void	renderMissile( const Missile& missile ) const;
	void	renderLeech( const LeechAttack& leech ) const;
	void	renderEnter( glm::vec2 center, glm::vec2 size ) const;
	void	renderPlayerSymbol( PLAYER_SELECTED player, glm::vec2 pos ) const;
	void	renderGrapple( const GrappleAttack& grapple ) const;
};

#endif RETRO_RENDERER_H
