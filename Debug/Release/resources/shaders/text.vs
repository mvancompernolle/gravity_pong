// Reference: http://www.learnopengl.com/#!In-Practice/2D-Game

#version 430 core

layout ( location = 0 ) in vec4 vsVert;
out vec2 fsTex;

uniform mat4 projection;

void main() {
	gl_Position = projection * vec4( vsVert.xy, 0.0, 1.0 );
	fsTex = vsVert.zw;
}