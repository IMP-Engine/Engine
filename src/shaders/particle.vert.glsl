#version 430


in vec3 position;
in int phase;
uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform mat4 projectionMatrix;

flat out vec3 color;
flat out vec3 centerPos;
flat out float particleSize;

void main() {
	particleSize = 150;
	gl_PointSize = particleSize / length(vec3(modelViewMatrix * vec4(position, 1.0)));
	gl_Position = modelViewProjectionMatrix * vec4(position, 1.0);
    color = vec3(0.89, 0.09, 0.05);

	centerPos = (modelViewMatrix * vec4(position, 1.0)).xyz;
}
