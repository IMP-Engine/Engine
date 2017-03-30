#version 430

in vec3 color;
in vec3 normalOut;

out vec4 outColor;


void main() {
    outColor = vec4(color, 1.0);
}
