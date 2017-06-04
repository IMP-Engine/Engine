#version 430

out vec3 newPos;

uniform sampler2DRect oldPos;

// TODO apply particle sleeping
void main() {
    newPos = texture(oldPos, gl_FragCoord.xy).xyz;
}