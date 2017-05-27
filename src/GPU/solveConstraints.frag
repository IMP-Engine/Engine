#version 430

out vec3 newPos;

uniform sampler2DRect oldPos;
uniform sampler2DRect invmasses;
uniform sampler2DRect constraints; 

void main() {
    vec3 pos = texture(oldPos, gl_FragCoord.xy).xyz;//texture() or texelFetc() ?
    newPos = pos;
}