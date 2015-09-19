#version 430 core

layout( location = 0 ) in vec4 vsVert;

out vec2 fsTex;

void main() {
	gl_Position = vec4( vsVert.xy, 0.0, 1.0 );
	fsTex = vsVert.zw;
}