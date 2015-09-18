#include "RetroRenderer.h"

#define PI 3.14159265359

RetroRenderer::RetroRenderer() {
	glClearColor( 0, 0, 0, 1 );
	glClear( GL_COLOR_BUFFER_BIT );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( -0.5, ( 1920 - 1 ) + 0.5, ( 1080 - 1 ) + 0.5, -0.5, 0.0, 1.0 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glColor3f( 1, 1, 1 );
}

RetroRenderer::~RetroRenderer() {

}

void RetroRenderer::renderCircle( glm::vec2 center, GLfloat radius, GLuint numLines ) const {
	glBegin( GL_LINE_LOOP );
	const GLfloat step = 2 * PI / numLines;

	for( GLfloat i = 0; i <= 2 * PI; i += step )
		glVertex2f( center.x + cos( i ) * radius, center.y + sin( i ) * radius );

	glEnd();
}

void RetroRenderer::renderRect( glm::vec2 topLeft, glm::vec2 bottomRight ) const {
	glBegin( GL_LINES );

	// top
	glVertex2f( topLeft.x, topLeft.y );
	glVertex2f( bottomRight.x, topLeft.y );
	// left
	glVertex2f( topLeft.x, topLeft.y );
	glVertex2f( topLeft.x, bottomRight.y );
	// right
	glVertex2f( bottomRight.x, topLeft.y );
	glVertex2f( bottomRight.x, bottomRight.y );
	// bottom
	glVertex2f( topLeft.x, bottomRight.y );
	glVertex2f( bottomRight.x, bottomRight.y );

	glEnd();
}

void RetroRenderer::renderTriangle( glm::vec2 p1, glm::vec2 p2, glm::vec2 p3 ) const {
	glBegin( GL_LINE_LOOP );
	glVertex2f( p1.x, p1.y );
	glVertex2f( p2.x, p2.y );
	glVertex2f( p3.x, p3.y );
	glEnd();
}

void RetroRenderer::renderLine( glm::vec2 v1, glm::vec2 v2 ) const {
	glBegin( GL_LINES );
	glVertex2f( v1.x, v1.y );
	glVertex2f( v2.x, v2.y );
	glEnd();
}

void RetroRenderer::renderGameBall( const GameBall& ball ) const {
	// render large outer ball
	renderCircle( ball.getCenter(), ball.radius, 16 );
	// render smaller inner ball
	renderCircle( ball.getCenter(), ball.radius * 0.5, 10 );

	if( ball.isLaunching && ball.launchDT < 1.0f ) {
		// render arrow
		glm::vec2 p1 = glm::vec2( ball.getCenter().x + cos( ball.dir ) * ball.radius * 1.2f, ball.getCenter().y + sin( ball.dir ) * ball.radius * 1.2f );
		glm::vec2 p2 = glm::vec2( ball.getCenter().x + cos( ball.dir ) * ball.radius * 2.0f, ball.getCenter().y + sin( ball.dir )* ball.radius * 2.0f );
		renderLine( p1, p2 );
		glm::vec3 right = glm::normalize( glm::cross( glm::vec3( p2, 0.0f ) - glm::vec3( p1, 0.0f ), glm::vec3( 0.0f, 0.0f, 1.0f ) ) );
		glm::vec2 p3 = glm::vec2( p2 - glm::vec2( right.x, right.y ) * 10.0f );
		glm::vec2 p4 = glm::vec2( p2 + glm::vec2( right.x, right.y ) * 10.0f );
		glm::vec2 p5 = glm::vec2( ball.getCenter().x + cos( ball.dir ) * ball.radius * 2.5f, ball.getCenter().y + sin( ball.dir )* ball.radius * 2.5f );
		renderTriangle( p3, p4, p5 );
	}
}

void RetroRenderer::renderPaddle( const PaddleObject& paddle ) const {
	// render two lines for front and back of the paddle
	GLfloat amountRounded = 0.08f;
	renderLine( glm::vec2( paddle.pos.x, paddle.pos.y + paddle.size.y * amountRounded ), glm::vec2( paddle.pos.x, paddle.pos.y + paddle.size.y * ( 1.0f - amountRounded ) ) );
	renderLine( glm::vec2( paddle.pos.x + paddle.size.x, paddle.pos.y + paddle.size.y * amountRounded ), glm::vec2( paddle.pos.x + paddle.size.x, paddle.pos.y + paddle.size.y * ( 1.0f - amountRounded ) ) );

	// render half circles on ends
	glBegin( GL_LINES );

	const GLfloat step = 2 * PI / 16;
	const GLfloat radius = paddle.size.x / 2.0f;
	for( float i = 0; i < PI - step; i += step ) {
		glVertex2f( paddle.getCenter().x + cos( i ) * radius, paddle.pos.y + radius - sin( i ) * radius );
		glVertex2f( paddle.getCenter().x + cos( i + step ) * radius, paddle.pos.y + radius - sin( i + step ) * radius );
	}

	for( float i = PI; i <= 2 * PI - step + .01; i += step ) {
		glVertex2f( paddle.getCenter().x + cos( i ) * radius, paddle.pos.y + paddle.size.y - radius - sin( i ) * radius );
		glVertex2f( paddle.getCenter().x + cos( i + step ) * radius, paddle.pos.y + paddle.size.y - radius - sin( i + step ) * radius );
	}

	glEnd();
}

void RetroRenderer::renderGravityBall( const GravityBall& gravBall ) const {
	renderCircle( gravBall.getCenter(), gravBall.radius, 16 );

	if( !gravBall.isReversed ) {
		renderLine( glm::vec2( gravBall.getCenter().x - gravBall.radius * 0.5f, gravBall.getCenter().y ), glm::vec2( gravBall.getCenter().x + gravBall.radius * 0.5f, gravBall.getCenter().y ) );
		renderLine( glm::vec2( gravBall.getCenter().x, gravBall.getCenter().y - gravBall.radius * 0.5f ), glm::vec2( gravBall.getCenter().x, gravBall.getCenter().y + gravBall.radius * 0.5f ) );
	} else {
		renderLine( glm::vec2( gravBall.getCenter().x - gravBall.radius * 0.5f, gravBall.getCenter().y ), glm::vec2( gravBall.getCenter().x + gravBall.radius * 0.5f, gravBall.getCenter().y ) );
	}
}

void RetroRenderer::renderMissile( const Missile& missile ) const {
	glm::vec2 rect[4];
	missile.getVertices( rect );

	// draw missile as rotated rectangle with point end
	glm::vec2 p1 = rect[0] + 0.7f * ( rect[1] - rect[0] );
	glm::vec2 p2 = rect[1] + 0.5f * ( rect[2] - rect[1] );
	glm::vec2 p3 = rect[3] + 0.7f * ( rect[2] - rect[3] );
	renderLine( rect[0], p1 );
	renderLine( p1, p2 );
	renderLine( p2, p3 );
	renderLine( p3, rect[3] );
	renderLine( rect[3], rect[0] );
}

void RetroRenderer::renderLeech( const LeechAttack& leech ) const {
	renderCircle( leech.getCenter(), leech.radius, 12 );
	renderLine( leech.getCenter(), glm::vec2( leech.getCenter().x + glm::normalize( leech.LAUNCH_DIRECTION.x ) * leech.radius * 1.25f, leech.getCenter().y ) );
}

void RetroRenderer::renderEnter( glm::vec2 center, glm::vec2 size ) const {
	renderRect( glm::vec2( center - size / 2.0f ), glm::vec2( center + size / 2.0f ) );
	renderLine( center, glm::vec2( center.x, center.y + size.y / 8.0f ) );
	renderLine( glm::vec2( center.x, center.y + size.y / 8.0f ), glm::vec2( center.x - size.x / 4.0f, center.y + size.y / 8.0f ) );
	glm::vec2 p1 = glm::vec2( center.x - size.x / 4.0f, center.y + size.y / 8.0f ) - glm::vec2( 0.0f, size.y * 0.1f );
	glm::vec2 p2 = glm::vec2( center.x - size.x / 4.0f, center.y + size.y / 8.0f ) - glm::vec2( size.x * 0.1f, 0.0f );
	glm::vec2 p3 = glm::vec2( center.x - size.x / 4.0f, center.y + size.y / 8.0f ) - glm::vec2( 0.0f, -size.y * 0.1f );
	renderTriangle( p1, p2, p3 );
}

void RetroRenderer::renderPlayerSymbol( PLAYER_SELECTED player, glm::vec2 pos ) const {
	if( player == P1_SELECTED ) {
		glm::vec2 p1 = pos;
		glm::vec2 p2 = glm::vec2( p1.x + 10.0f, p1.y );
		glm::vec2 p3 = glm::vec2( p2.x, p2.y + 20.0f );
		glm::vec2 p4 = glm::vec2( p3.x - 10.0f, p3.y );
		glm::vec2 p5 = glm::vec2( p3.x + 10.0f, p3.y );
		renderLine( p1, p2 );
		renderLine( p2, p3 );
		renderLine( p4, p5 );
	} else if( player == P2_SELECTED ) {
		glm::vec2 p1 = pos;
		glm::vec2 p2 = glm::vec2( p1.x + 20.0f, p1.y );
		glm::vec2 p3 = glm::vec2( p2.x, p2.y + 10.0f );
		glm::vec2 p4 = glm::vec2( p3.x - 20.0f, p3.y );
		glm::vec2 p5 = glm::vec2( p4.x, p3.y + 10.0f );
		glm::vec2 p6 = glm::vec2( p4.x + 20.0f, p5.y );
		renderLine( p1, p2 );
		renderLine( p2, p3 );
		renderLine( p3, p4 );
		renderLine( p4, p5 );
		renderLine( p5, p6 );
	}
}

void RetroRenderer::renderGrapple( const GrappleAttack& grapple ) const {
	/*// render links
	for ( int i = 0; i < grapple.NUM_ANCHORS - 1; ++i ) {
		glm::vec2 diff = glm::normalize(grapple.anchors[i + 1].getCenter() - grapple.anchors[i].getCenter());
		renderLine( grapple.anchors[i].getCenter() + diff * grapple.anchors[i].radius, grapple.anchors[i+1].getCenter() - diff * grapple.anchors[i+1].radius );
	}

	// render anchors
	for ( const BallObject& anchor : grapple.anchors ) {
		renderCircle( anchor.getCenter(), anchor.radius, 10 );
	}*/
}