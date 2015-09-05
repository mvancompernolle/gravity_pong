#version 430 core

layout (location = 0) in vec4 vsVert;

out vec2 fsTex;
out vec4 fsParticleColor;

uniform mat4 projection;
uniform vec2 offset;
uniform vec4 color;

void main() {
	float scale = 30.0;
	fsTex = vsVert.zw;
	fsParticleColor = color;
	gl_Position = projection * vec4( ( vsVert.xy * scale ) + offset, 0.0, 1.0 );
}