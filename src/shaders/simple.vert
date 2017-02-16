#version 430

uniform mat4 MVP;
uniform mat4 MV;
uniform mat4 normalMatrix;

//in vec3 vCol;
in vec3 vPos;

out vec3 color;
out vec3 viewSpacePos;

void main() {
	viewSpacePos = (MV * vec4(vPos, 1)).xyz;
    gl_Position = MVP * vec4(vPos, 1.0);
    color = vec3(1.0, 1.0, 1.0); //vCol;
}
