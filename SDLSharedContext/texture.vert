#version 330

layout (location = 0) in vec4 a_position;
layout (location = 1) in vec2 a_texcoord;

out vec2 UV;

void main(void) {
    gl_Position = a_position;
	UV = a_texcoord;
}
