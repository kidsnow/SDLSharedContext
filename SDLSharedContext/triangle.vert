#version 330

uniform mat4 u_MVPMat;

layout (location = 0) in vec4 a_position;

void main(void) {
    gl_Position = u_MVPMat * a_position;
}
