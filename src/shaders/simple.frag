#version 430

in vec3 color;
in vec3 position;
in vec3 normalOut;

uniform vec3 lightPos;

out vec4 outColor;

void main() {
    float diffuse = max(0.0, dot(normalize(lightPos - position), normalOut));
    outColor = vec4(min(1.0, 0.1 + diffuse) * color, 1.0);
}
