#version 430

in vec3 color;
in vec3 position;
in vec3 normalOut;

uniform vec3 viewSpaceLightPos;

out vec4 outColor;


void main() {
    float diffuse = max(0.0, dot(normalize(viewSpaceLightPos - position), normalOut));
    outColor = vec4(min(1.0, 0.2 + diffuse) * color, 1.0);
}
