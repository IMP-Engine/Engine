#version 430

out vec3 velocity;

uniform sampler2DRect oldPos;
uniform sampler2DRect pos;

uniform float dt;

void main() {
    velocity = (texture(pos, gl_FragCoord.xy).xyz - texture(oldPos, gl_FragCoord.xy).xyz) / dt;
}