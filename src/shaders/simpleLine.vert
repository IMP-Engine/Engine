#version 430

uniform mat4 modelViewProjectionMatrix;

//in vec3 vCol;
in vec3 vPos;

out vec3 color;

void main() {
    gl_Position = modelViewProjectionMatrix * vec4(vPos, 1.0);
    color = vec3(0, 0, 0); //vCol;
}
