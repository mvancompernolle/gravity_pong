#include "SpriteRenderer.h"

// Reference: http://www.learnopengl.com/#!In-Practice/2D-Game/Rendering-Sprites

SpriteRenderer::SpriteRenderer(Shader& shader) {
	this->shader = shader;
	initRenderData();
}

SpriteRenderer::~SpriteRenderer() {
	glDeleteVertexArrays(1, &VAO);
}

void SpriteRenderer::drawSprite( const Texture& texture, glm::vec2 position, glm::vec2 size, GLfloat rotate, glm::vec4 color, GLboolean flipY ) {
	shader.use();
	glm::mat4 model;

	// translate (transformation order: scale, rotate, translate but in reversed order)
	model = glm::translate(model, glm::vec3(position, 0.0f));

	// need to translate to center of quad before rotating
	model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
	// rotate the sprite along the z axis
	model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f));
	// move the origin back
	model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

	// scale the model
	model = glm::scale(model, glm::vec3(size, 1.0f));

	shader.setMatrix4("model", model);
	shader.setInteger( "flipTexture", flipY );

	// render the sprite
	shader.setVector4f("spriteColor", color);
	glActiveTexture(GL_TEXTURE0);
	texture.bind();

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void SpriteRenderer::drawSprite( const Texture& texture, glm::mat4 model, glm::vec4 color ) {
	shader.use();
	shader.setMatrix4( "model", model );
	shader.setVector4f( "spriteColor", color );
	glActiveTexture( GL_TEXTURE0 );
	texture.bind();

	glBindVertexArray( VAO );
	glDrawArrays( GL_TRIANGLES, 0, 6 );
	glBindVertexArray( 0 );
}

void SpriteRenderer::initRenderData() {
	// create vao and vbo
	GLuint VBO;
	GLfloat vertices[] = {
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}