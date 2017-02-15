//#version 430


attribute vec3 position;
attribute float phase;

varying vec3 color;
//varying flat vec3 center;
void main() {
	gl_Position = vec4(position, 1.0);
	//center = vec3(1, 1, 1);
    color = vec3(0,0,1);
}