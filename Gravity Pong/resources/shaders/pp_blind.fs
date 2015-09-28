#version 430 core

in vec2 fsTex;
out vec4 color;

uniform sampler2D scene;

uniform vec2 heightRange, widthRange;
uniform float visionRadius;
uniform vec2 playerPos;
uniform float fadeInLine;

void main() {
	if( gl_FragCoord.x >= widthRange.x && gl_FragCoord.x <= widthRange.y && gl_FragCoord.y >= heightRange.x && gl_FragCoord.y <= heightRange.y ){
		float dist = distance( playerPos, gl_FragCoord.xy );
		float brightnessVal = min( max( 1.0 - abs( gl_FragCoord.x - fadeInLine ) / 100.0 , 0.0 ) + max( 1.0 - dist / visionRadius, 0.0 ), 1.0 );
		color = texture( scene, fsTex ) * vec4( vec3( brightnessVal ), 1.0 );
	} else {
		color = texture( scene, fsTex );
	}
}