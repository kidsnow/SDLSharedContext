#version 330

layout (location = 0) out vec4 final_color;

uniform float u_color;

void main(void) {
    final_color = vec4(u_color, 1.0f, u_color, 1.0f);
}
