#version 430


in vec3 position;
in float particleSize;
in int phase;
uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform mat4 projectionMatrix;
uniform int height;

flat out vec3 color;
flat out vec3 centerPos;
flat out float particleSizeOut;

vec3 colors[10];

void main() {

    
    colors[0] = vec3(190,115,215) / 255;
    colors[1] = vec3(219,222,52) / 255;
    colors[2] = vec3(16,171,255) / 255;
    colors[3] = vec3(46,186,0) / 255;
    colors[4] = vec3(1,217,228) / 255;
    colors[5] = vec3(240,45,51) / 255;
    colors[6] = vec3(244,93,28) / 255;
    colors[7] = vec3(255,196,100) / 255;
    colors[8] = vec3(246,70,201) / 255;
    colors[9] = vec3(1,1,0);



    particleSizeOut = particleSize;
    gl_Position = modelViewProjectionMatrix * vec4(position, 1.0);
    gl_PointSize = height * 2*particleSize / gl_Position.w;

    int c = int(mod(phase,10));
    if (0 == 1) {
        color = vec3(0.89, 0.09, 0.05);
    } else {
        color = colors[c];
    }



    centerPos = (modelViewMatrix * vec4(position, 1.0)).xyz;
}
