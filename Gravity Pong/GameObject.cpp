#include "GameObject.h"

// REFERENCE: http://learnopengl.com/#!In-Practice/2D-Game/Levels

const float PI = 3.14159265358;

GameObject::GameObject() : pos( 0, 0 ), size( 1, 1 ), vel( 0.0f ), color( 1.0f ), rotation( 0.0f ), sprite(), isSolid( false ), isDestroyed( false ), mass( 50.0f ) {

}

GameObject::GameObject( glm::vec2 pos, glm::vec2 size, Texture sprite, glm::vec4 color, glm::vec2 vel )
	: pos( pos ), size( size ), color( color ), rotation( 0.0f ), vel( vel ), sprite( sprite ), isSolid( false ), isDestroyed( false ), mass( 50.0f ) {

}

GameObject::~GameObject() {

}


void GameObject::draw( SpriteRenderer& renderer ) {
	renderer.drawSprite( sprite, pos, size, rotation, color );
}

glm::vec2 GameObject::getCenter() const {
	return glm::vec2( pos + size / 2.0f );
}

void GameObject::getVertices( glm::vec2 vertices[4] ) const {
	// get non rotated vertices
	vertices[0] = glm::vec2( pos.x, pos.y );
	vertices[1] = glm::vec2( pos.x + size.x, pos.y );
	vertices[2] = glm::vec2( pos.x + size.x, pos.y + size.y );
	vertices[3] = glm::vec2( pos.x, pos.y + size.y );

	if( rotation != 0.0f ) {
		GLfloat tmpX, tmpY;
		GLfloat rot = glm::radians(rotation);
		for( int i = 0; i < 4; ++i ) {
			// rotate vertex with respect to origin
			vertices[i] -= getCenter();
			tmpX = vertices[i].x * std::cos( rot ) - vertices[i].y * std::sin( rot );
			tmpY = vertices[i].x * std::sin( rot ) + vertices[i].y * std::cos( rot );
			vertices[i].x = tmpX;
			vertices[i].y = tmpY;
			vertices[i] += getCenter();
		}
	}
}