#version 430

uniform mat4 modelViewProjectionMatrix;

//in vec3 vCol;
in vec3 vPos;
in vec3 normal;

out vec3 color;
out vec3 normalOut;

void main() {
    gl_Position = modelViewProjectionMatrix * vec4(vPos, 1.0);
    color = vec3(0.64, 0.64, 0.64); //vCol;
    normalOut = normal;
}
