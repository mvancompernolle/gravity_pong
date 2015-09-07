#version 430 core

layout (location = 0) in vec4 vsVert;

out vec2 fsTex;
out vec4 fsParticleColor;

uniform mat4 projection, model;
uniform vec2 offset;
uniform vec4 color;
uniform float scale;

void main() {
	fsTex = vsVert.zw;
	fsParticleColor = color;
	gl_Position = projection * model * vec4( ( vsVert.xy * scale ) + offset, 0.0, 1.0 );
}