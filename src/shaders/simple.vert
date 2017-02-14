uniform mat4 MVP;
uniform mat4 MV;
uniform mat4 normalMatrix;
//attribute vec3 vCol;
attribute vec3 vPos;

varying vec3 color;

out vec3 viewSpaceNormal;
out vec3 viewSpacePos;

void main() {
	viewSpaceNormal = (normalMatrix * vec4(gl_Normal.xyz, 0.0)).xyz;
	viewSpacePos = (MV * vec4(vPos, 1)).xyz;

    gl_Position = MVP * vec4(vPos, 1.0);
    color = viewSpaceNormal;//gl_Normal.xyz; //vec3(1.0f, 1.0f, 1.0f); //vCol;
}
