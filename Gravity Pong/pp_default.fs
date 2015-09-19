#version 430 core

in vec2 fsTex;
out vec4 color;

uniform sampler2D scene;

void main() {
	color = texture( scene, fsTex );
}