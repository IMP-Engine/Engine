#version 430


in vec3 position;
in int phase;
uniform mat4 modelViewProjectionMatrix;


out vec3 color;

void main() {
	gl_Position = modelViewProjectionMatrix * vec4(position, 1.0);
    color = vec3(0,0,1);
}