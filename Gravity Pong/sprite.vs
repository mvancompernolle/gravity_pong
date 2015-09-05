#version 430 core
layout(location = 0) in vec4 vs_vert;

out vec2 fs_tex;

uniform mat4 model;
uniform mat4 projection;

void main() {
	fs_tex = vs_vert.zw;
	gl_Position = projection * model * vec4(vs_vert.xy, 0.0, 1.0);
}