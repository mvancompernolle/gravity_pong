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
	GLfloat		life, rotation;

	Particle() : pos( 0.0f ), vel( 0.0f ), color( 1.0f ), life( 0.0f ), rotation( 0.0f ) {}
};

class ParticleGenerator {
public:
	GLfloat					particleSize;

							ParticleGenerator( Texture texture, GLuint amount, GLfloat particleSize = 10.0f );
							~ParticleGenerator();
	void					addParticles( glm::vec2 pos, GLuint newParticles, glm::vec3 color = glm::vec3( 1.0f ), GLfloat rotation = 0.0f);
	void					update( GLfloat dt );
	void					draw( SpriteRenderer& renderer );
	std::vector<Particle>	getParticles() const;

private:
	std::vector<Particle>	particles;
	GLuint					amount;
	GLuint					VAO;
	Texture					texture;
	GLuint					lastUsedParticle;
	
	void	init();
	GLuint	firstUnusedParticle();
	void	respawnParticle( Particle& particle, const glm::vec2 pos, const glm::vec3 color, const GLfloat rotation = 0.0f );
};

#endif // PARTICLE_GENERATOR_H

