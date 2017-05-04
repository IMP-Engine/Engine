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

vec3 colors[21];

void main() {

    colors[ 0] = vec3(0.89, 0.09, 0.05);

    colors[ 1] = vec3(104,255, 75) / 255;
    colors[ 2] = vec3(246,  0,147) / 255;
    colors[ 3] = vec3(  2,242,124) / 255;
    colors[ 4] = vec3(255,134,255) / 255;
    colors[ 5] = vec3( 53,108,  0) / 255;
    colors[ 6] = vec3(  0, 82,199) / 255;
    colors[ 7] = vec3(232,185,  0) / 255;
    colors[ 8] = vec3(  0, 13, 80) / 255;
    colors[ 9] = vec3(205,255,171) / 255;
    colors[10] = vec3(235,  0, 59) / 255;
    colors[11] = vec3(  2,153,164) / 255;
    colors[12] = vec3(255,139, 98) / 255;
    colors[13] = vec3(  0, 41, 21) / 255;
    colors[14] = vec3(255,121,195) / 255;
    colors[15] = vec3( 96, 57,  0) / 255;
    colors[16] = vec3(152,181,255) / 255;
    colors[17] = vec3( 85,  0,  1) / 255;
    colors[18] = vec3(255,227,242) / 255;
    colors[19] = vec3( 47,  0, 36) / 255;
    colors[20] = vec3(255,187,158) / 255;



    particleSizeOut = particleSize;
    gl_Position = modelViewProjectionMatrix * vec4(position, 1.0);
    gl_PointSize = height * 2*particleSize / gl_Position.w;

    int c = int(mod(phase,21));
    color = colors[c];



    centerPos = (modelViewMatrix * vec4(position, 1.0)).xyz;
}
