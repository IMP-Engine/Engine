#version 430

in vec3 color;

uniform vec3 normal;

out vec4 outColor;


void main() {
    outColor = vec4(color, 1.0);
}
