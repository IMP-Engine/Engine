#version 430


in vec3 position;
in int phase;
uniform mat4 modelViewProjectionMatrix;


out vec3 color;

void main() {
	gl_Position = modelViewProjectionMatrix * vec4(position, 1.0);
    if (phase == 1) {
        color = vec3(0, 0, 1);
    }
    else {
        color = vec3(1, 0, 0);
    }
}