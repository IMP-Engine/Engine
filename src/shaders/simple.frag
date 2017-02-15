in vec3 color;
in vec3 viewSpacePos;

void main() {
    gl_FragColor = vec4(color, 1.0);
}
