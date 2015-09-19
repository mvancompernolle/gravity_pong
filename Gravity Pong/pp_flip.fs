#version 430 core

in vec2 fsTex;
out vec4 color;

uniform sampler2D scene;

uniform vec2 heightRange;
uniform vec2 widthRange;
uniform float maxWidth;

int borderSize = 3;

void main() {
	if( gl_FragCoord.y >= heightRange.x && gl_FragCoord.y <= heightRange.y ){
		// pixels are not part of gui
		if( gl_FragCoord.x >= widthRange.x && gl_FragCoord.x <= widthRange.y ){
			// pixels are in flip range
			float flipSizeX = widthRange.y - widthRange.x;
			float xPercent = ( widthRange.y - gl_FragCoord.x ) / flipSizeX;
			color = texture( scene, vec2( ( widthRange.x + xPercent * flipSizeX ) / maxWidth, fsTex.y ) );
		} else if ( ( widthRange.x > 0.0 && ( gl_FragCoord.x < widthRange.x && gl_FragCoord.x >= widthRange.x - borderSize ) )
			|| ( widthRange.y < maxWidth && ( gl_FragCoord.x > widthRange.y && gl_FragCoord.x <= widthRange.y + borderSize ) ) ){
			// pixels are the inner border
			color = vec4( 1.0f, 0.0f, 0.0f, 0.5f );
		} else {
			// pixels are not in flip range
			color = texture( scene, fsTex );
		}
	} else {
		// pixels are part of gui
		color = texture( scene, fsTex );
	}
}