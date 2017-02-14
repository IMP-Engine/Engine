varying vec3 color;
uniform vec3 lightPos;

in vec3 viewSpaceNormal;
in vec3 viewSpacePos;


vec3 direcIllumination(vec3 wo, vec3 n) {
	return vec3(0);
}

vec3 indirectIllumination(vec3 wo, vec3 n) {
	return vec3(0);
}


void main() {
	vec3 wo = normalize(-viewSpacePos);
	vec3 n = normalize(viewSpaceNormal);

    gl_FragColor = vec4(color, 1.0); // vec4(viewSpaceNormal, 1.0)
}
