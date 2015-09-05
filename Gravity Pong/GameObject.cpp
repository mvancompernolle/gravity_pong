#include "GameObject.h"


GameObject::GameObject() : pos(0, 0), size(1, 1), vel(0.0f), color(1.0f), rotation(0.0f), sprite(), isSolid(false), isDestroyed(false) {

}

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture sprite, glm::vec3 color, glm::vec2 vel )
	: pos(pos), size(size), color(color), rotation(0.0f), vel( vel ), sprite(sprite), isSolid(false), isDestroyed(false) {

}

GameObject::~GameObject() {

}


void GameObject::draw(SpriteRenderer& renderer) {
	renderer.drawSprite(sprite, pos, size, rotation, color);
}

glm::vec2 GameObject::getCenter() const {
	return glm::vec2( pos + size / 2.0f );
}