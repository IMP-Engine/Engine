#version 430

out vec3 newPos;

uniform sampler2DRect posStart;
uniform sampler2DRect velocity;

uniform vec3 forces;
uniform float dt;

void main() {
    vec3 updatedVelocity = texture(velocity, gl_FragCoord.xy).xyz + dt * forces;
    newPos = texture(posStart, gl_FragCoord.xy).xyz + dt * updatedVelocity;
}