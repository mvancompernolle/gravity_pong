#include "ParticleGenerator.h"

#include <iostream>

ParticleGenerator::ParticleGenerator( const Shader shader, const Texture texture, const GLuint amount, GLfloat particleSize )
	: shader(shader), texture(texture), amount(amount), lastUsedParticle(0), particleSize( particleSize ) {
	init();
}

ParticleGenerator::~ParticleGenerator() {
	
}

void ParticleGenerator::addParticles( GameObject object, GLuint newParticles, glm::vec2 offset, GLfloat rotation ) {
	// add new particles
	for( GLuint i = 0; i < newParticles; ++i ) {
		int unusedParticle = firstUnusedParticle();
		respawnParticle( particles[unusedParticle], object, offset, rotation );
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

void ParticleGenerator::draw() {
	// use additive blending to make particles glow
	glBlendFunc( GL_SRC_ALPHA, GL_ONE );
	shader.use();
	for( Particle particle : particles ) {
		//std::cout << "drawing " << std::endl;
		if( particle.life > 0.0f ) {
			shader.setVector2f( "offset", particle.pos );
			shader.setVector4f( "color", particle.color );
			shader.setFloat( "scale", particleSize );
			glm::mat4 model;
			model = glm::rotate( model, particle.rotation, glm::vec3( 0.0f, 0.0f, 1.0f ) );
			shader.setMatrix4( "model", model );
			texture.bind();
			glBindVertexArray( VAO );
			glDrawArrays( GL_TRIANGLES, 0, 6 );
			glBindVertexArray( 0 );
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

void ParticleGenerator::respawnParticle( Particle& particle, const GameObject& object, const glm::vec2 offset, const GLfloat rotation ) {
	GLfloat random = ( ( rand() % 100 ) - 50 ) / 10.0f;
	GLfloat rColor = 0.5 + ( ( rand() % 100 ) / 100.0f );
	particle.pos = object.pos + random + offset;
	particle.color = glm::vec4( rColor, rColor, rColor, 1.0f );
	particle.life = 1.0f;
	particle.vel = object.vel * 0.1f;
	particle.rotation = rotation;
}

std::vector<Particle> ParticleGenerator::getParticles() const {
	return particles;
}
