#version 430 core

in vec2 fsTex;
out vec4 color;

uniform sampler2D scene;

uniform vec2 heightRange;
uniform float visionRadius;
uniform vec2 playerPos;

void main() {
	if( gl_FragCoord.y >= heightRange.x && gl_FragCoord.y <= heightRange.y ){
		float dist = distance( playerPos, gl_FragCoord.xy );
		if( dist < visionRadius ) {
			color = texture( scene, fsTex ) * vec4( vec3( 0.6 - dist / visionRadius ), 1.0 );
		} else {
			color = texture( scene, fsTex );
		}
	} else {
		color = texture( scene, fsTex );
	}
}