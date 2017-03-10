#version 430


in vec3 position;
in int phase;
uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform mat4 projectionMatrix;

out vec3 color;
flat out vec3 centerPos;
flat out float particleSize;

void main() {
	particleSize = 0.1;
	gl_Position = modelViewProjectionMatrix * vec4(position, 1.0);
    color = vec3(0.89, 0.09, 0.05);

	vec4 fragPos = modelViewMatrix * vec4(position.x, position.y, 0.5, 1);
	vec4 projCorner = projectionMatrix * vec4(particleSize, particleSize, fragPos.z, fragPos.w);
	gl_PointSize = 1280 * projCorner.x / projCorner.w;
	//gl_Position = projectionMatrix * fragPos;

	centerPos = (modelViewMatrix * vec4(position, 1.0)).xyz;
}
