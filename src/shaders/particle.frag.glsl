#version 430

in vec2 gl_PointCoord;

in vec3 color;
flat in vec3 centerPos;
flat in float particleSize;
out vec4 outColor;

uniform vec3 viewSpaceLightPos;

struct Ray 
{
	vec3 o, d;
	float maxt;
} ray;

float lengthSquared(in vec3 v) {
	return dot(v,v);
}


void main() {
	vec2 pointCoord = gl_PointCoord - vec2(0.5);
	float length = length(pointCoord);
	if (length > 0.5) discard; // Keep the particle round

	vec3 normal = normalize(vec3(pointCoord, 0.5-length));
	vec3 fragPos = centerPos + (particleSize) * vec3(normal.x, -normal.y, -normal.z);
	float diffuse = max(0.0, dot(normalize(viewSpaceLightPos - fragPos), normal));

	outColor = vec4((0.2 + diffuse) * color, 1.0);
}