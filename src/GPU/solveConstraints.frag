#version 430

out vec3 pos;

void main() {
    pos = vec3(gl_FragCoord.xy, 0);
}