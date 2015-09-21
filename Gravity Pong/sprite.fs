// Reference: http://www.learnopengl.com/#!In-Practice/2D-Game

#version 430 core
in vec2 fs_tex;
out vec4 color;

uniform sampler2D image;
uniform vec4 spriteColor;

void main() {
	color = spriteColor * texture(image, fs_tex);
}