// Reference: http://www.learnopengl.com/#!In-Practice/2D-Game

#version 430 core
layout(location = 0) in vec4 vs_vert;

out vec2 fs_tex;

uniform mat4 model;
uniform mat4 projection;
uniform bool flipTexture = true;

void main() {
	vec2 tex = vs_vert.zw;
	if( flipTexture ) {
		tex.x = 1.0 - tex.x;
	}
	fs_tex = tex;
	gl_Position = projection * model * vec4(vs_vert.xy, 0.0, 1.0);
}