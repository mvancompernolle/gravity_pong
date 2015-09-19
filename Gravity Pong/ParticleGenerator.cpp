#include "ParticleGenerator.h"

#include <iostream>

ParticleGenerator::ParticleGenerator( const Texture texture, const GLuint amount, GLfloat particleSize )
	: texture(texture), amount(amount), lastUsedParticle(0), particleSize( particleSize ) {
	init();
}

ParticleGenerator::~ParticleGenerator() {
	
}

void ParticleGenerator::addParticles(glm::vec2 pos, GLuint newParticles, glm::vec3 color, GLfloat rotation ) {
	// add new particles
	for( GLuint i = 0; i < newParticles; ++i ) {
		// find an unused particle in the array
		int unusedParticle = firstUnusedParticle();
		// spawn a new particle at that index
		respawnParticle( particles[unusedParticle], pos, color, rotation );
	}
}

void ParticleGenerator::update( GLfloat dt ) {
	// update all particles
	for( GLuint i = 0; i < amount; ++i ) {
		Particle& p = particles[i];
		p.life -= dt;
		if( p.life > 0.0f ) {
			p.pos -= p.vel * dt;
			p.color.a -= dt * 2.5;
		}
	}
}

void ParticleGenerator::draw(SpriteRenderer& renderer) {
	// use additive blending to make particles glow
	glBlendFunc( GL_SRC_ALPHA, GL_ONE );
	for( Particle particle : particles ) {
		//std::cout << "drawing " << std::endl;
		if( particle.life > 0.0f ) {
			renderer.drawSprite(texture, particle.pos, glm::vec2(particleSize), particle.rotation, particle.color );
		}
	}
	// reset blending mode
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

void ParticleGenerator::init() {
	GLuint VBO;
	GLfloat particleQuad[] = {
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};
	glGenVertexArrays( 1, &VAO );
	glGenBuffers( 1, &VBO );
	glBindVertexArray( VAO );

	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	glBufferData( GL_ARRAY_BUFFER, sizeof( particleQuad ), particleQuad, GL_STATIC_DRAW );

	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof( GLfloat ), (GLvoid*) 0 );
	glBindVertexArray( 0 );

	// create amount of defualt particles
	for( GLuint i = 0; i < amount; ++i ) {
		particles.push_back( Particle() );
	}
}

GLuint ParticleGenerator::firstUnusedParticle() {
	// search from last used particle
	for( GLuint i = lastUsedParticle; i < amount; ++i ) {
		if( particles[i].life <= 0.0f ) {
			lastUsedParticle = i;
			return i;
		}
	}
	// otherwise do a linear search
	for( GLuint i = 0; i < lastUsedParticle; ++i ) {
		if( particles[i].life <= 0.0f ) {
			lastUsedParticle = i;
			return i;
		}
	}
	// all particles are taken, override the first one
	lastUsedParticle = 0;
	return 0;
}

void ParticleGenerator::respawnParticle( Particle& particle, const glm::vec2 pos, const glm::vec3 color, const GLfloat rotation ) {
	// randomly pick color between white and gray
	GLfloat rColor = 0.5 + ( ( rand() % 50 ) / 100.0f );
	particle.pos = pos + glm::vec2(-5 + rand() % 10, -5 + rand() % 10);
	particle.color = glm::vec4( rColor, rColor, rColor, 1.0f ) * glm::vec4(color, 1.0f);
	particle.life = 1.0f;
	particle.rotation = rotation;
}

std::vector<Particle> ParticleGenerator::getParticles() const {
	return particles;
}
