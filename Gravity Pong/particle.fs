#version 430 core
in vec2 fsTex;
in vec4 fsParticleColor;
out vec4 color;

uniform sampler2D sprite;

void main() {
	color = ( texture( sprite, fsTex ) * fsParticleColor );
}