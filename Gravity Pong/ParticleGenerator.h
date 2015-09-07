#pragma once

#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H

#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Shader.h"
#include "Texture.h"
#include "GameObject.h"

struct Particle {
	glm::vec2	pos, vel;
	glm::vec4	color;
	GLfloat		life;

	Particle() : pos( 0.0f ), vel( 0.0f ), color( 1.0f ), life( 0.0f ) {}
};

class ParticleGenerator {
public:
	GLfloat					particleSize;

							ParticleGenerator( Shader shader, Texture texture, GLuint amount, GLfloat particleSize = 10.0f );
							~ParticleGenerator();
	void					addParticles( GameObject object, GLuint newParticles, glm::vec2 offset = glm::vec2( 0.0f, 0.0f ));
	void					update( GLfloat dt );
	void					draw();

private:
	std::vector<Particle>	particles;
	GLuint					amount;
	Shader					shader;
	GLuint					VAO;
	Texture					texture;
	GLuint					lastUsedParticle;
	
	void	init();
	GLuint	firstUnusedParticle();
	void	respawnParticle( Particle& particle, const GameObject& object, const glm::vec2 offset = glm::vec2( 0.0f, 0.0f ) );
};

#endif // PARTICLE_GENERATOR_H

