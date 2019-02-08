#version 330

layout (location = 0) out vec4 final_color;

in vec2 UV;
uniform sampler2D u_renderedTexture;

void main(void) {
    final_color = texture(u_renderedTexture, UV);
	//float temp = UV.x * 0.5f + UV.y * 0.5f;
	//final_color = vec4(temp, temp, temp, 1.0f);
}
