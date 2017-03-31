#version 430
#define M_PI 3.14159265358979323846264338327950288 /* pi */

in vec2 gl_PointCoord;

flat in vec3 color;
flat in vec3 centerPos;
flat in float particleSizeOut;

out vec4 outColor;

uniform vec3 viewSpaceLightPos;

void main() {
    // Get fragments 2D-coordinate on point in [-1, 1]
    vec2 pointCoord = vec2(2 * (gl_PointCoord.x - 0.5), 2 * (-gl_PointCoord.y + 0.5));
    // Get distance from middle of point to fragment
    float length = length(pointCoord);
    if (length > 1.0) discard; // Keep the particle round

                               // Get normal of fragment on the imagined sphere
    vec3 normal = normalize(vec3(pointCoord, cos(length * M_PI / 2)));
    // Get fragment position in viewspace
    vec3 fragPos = centerPos + particleSizeOut * vec3(normal.x, -normal.y, -normal.z);
    // Calculate direct diffuse lighting
    float diffuse = max(0.0, dot(normalize(viewSpaceLightPos - fragPos), normal));

    outColor = vec4((0.2 + diffuse) * color, 1.0);

}
