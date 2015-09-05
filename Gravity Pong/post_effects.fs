#version 430 core

in vec2 fsTex;
out vec4 color;

uniform sampler2D scene;
uniform vec2 offsets[9];
uniform int edgeKernel[9];
uniform float blurKernel[9];

uniform bool chaos;
uniform bool confuse;
uniform bool shake;

void main() {
	color = vec4( 0.0, 0.0, 0.0, 1.0 );
	vec3 samples[9];

	// smaple from texture offset if using convolution matrix
	if( chaos || shake ) {
		for( int i = 0; i < 9; ++i ) {
			samples[i] = vec3( texture( scene, fsTex.st + offsets[i] ) );
		}
	}
	// process effects
	if( chaos ) {
		for( int i = 0; i < 9; ++i ) {
			color += vec4( samples[i] * edgeKernel[i], 0.0 );
		}
		color.a = 1.0;
	} else if( confuse ) {
		color = vec4( 1.0 - texture( scene, fsTex ).rgb, 1.0 );
	} else if( shake ) {
		for( int i = 0; i < 9; ++i ) {
			color += vec4( samples[i] * blurKernel[i], 0.0 );
		}
		color.a = 1.0;
	} else {
		color = texture( scene, fsTex );
	}
}