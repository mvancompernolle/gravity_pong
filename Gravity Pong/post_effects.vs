#version 430 core

layout( location = 0 ) in vec4 vsVert;

out vec2 fsTex;

uniform bool chaos;
uniform bool confuse;
uniform bool shake;
uniform float time;

void main() {
	gl_Position = vec4( vsVert.xy, 0.0, 1.0 );
	vec2 texture = vsVert.zw;

	if( chaos ) {
		float strength = 0.3;
		vec2 pos = vec2( texture.x + sin( time ) * strength, texture.y + cos( time ) * strength );
		fsTex = pos;
	} else if( confuse ) {
		fsTex = vec2( 1.0 - texture.x, 1.0 - texture.y );
	} else {
		fsTex = texture;
	}
	if( shake ) {
		float strength = 0.01;
		gl_Position.xy += vec2( cos( time * 10 ) * strength, cos( time * 15 ) * strength );
	} 
}