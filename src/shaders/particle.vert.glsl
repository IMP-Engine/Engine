#version 430


in vec3 position;
in int phase;
in float particleSize;
uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform mat4 projectionMatrix;
uniform int height;

flat out vec3 color;
flat out vec3 centerPos;
flat out float particleSizeOut;

void main() {
    particleSizeOut = particleSize;
    gl_Position = modelViewProjectionMatrix * vec4(position, 1.0);
    gl_PointSize = height * 2*particleSize / gl_Position.w;
    color = vec3(0.89, 0.09, 0.05);

    centerPos = (modelViewMatrix * vec4(position, 1.0)).xyz;
}
