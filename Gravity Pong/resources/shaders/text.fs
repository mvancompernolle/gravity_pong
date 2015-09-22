// Reference: http://www.learnopengl.com/#!In-Practice/2D-Game

#version 430 core
in vec2 fsTex;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main() {
	vec4 sampled = vec4( 1.0, 1.0, 1.0, texture( text, fsTex ).r );
	color = vec4( textColor, 1.0 ) * sampled;
}