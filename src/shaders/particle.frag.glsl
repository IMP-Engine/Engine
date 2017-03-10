#version 430
#define M_PI 3.14159265358979323846264338327950288 /* pi */

in vec2 gl_PointCoord;

flat in vec3 color;
flat in vec3 centerPos;
flat in float particleSize;
out vec4 outColor;

uniform vec3 viewSpaceLightPos;

struct Ray 
{
	vec3 o, d;
	float maxt;
} ray;

float lengthSquared(in vec3 v) {
	return dot(v,v);
}

/*
bool sphereIntersect(in vec3 center, in float radius, inout Ray r, out vec3 position, out vec3 normal) {
    vec3 pToC = center - ray.o;
    float r2 = radius*radius;
    vec3 cProj = ray.o + dot(ray.d, pToC) * ray.d;


    if (dot(pToC, ray.d) < 0) {
        if (lengthSquared(pToC) > r2) {
            return false;
        }
        else if (lengthSquared(pToC) == r2) {
            position = cProj;
            normal = -normalize(position - center);

            
            if (dot(ray.d, normal) > 0) {
                normal *= -1;
            }
            
            position += 0.0001f * normal;
            ray.maxt = 0;
			return true;
        }

        // occurs when p is inside the sphere
        float dist = sqrt(r2 - lengthSquared(cProj - center));
        float di = dist - length(cProj - ray.o);

        position = ray.o + ray.d * di;
        // We're inside the sphere, so the normal is pointing towards the center of the sphere.
        normal = normalize(position-center);
        position += 0.0001f * normal;
        ray.maxt = di;
		return true;
    }

    float distance = lengthSquared(cProj - center);

    if (distance == r2) {
        
        position = cProj;
        normal = normalize(position - center);
        if (dot(ray.d, normal) > 0) {
            normal *= -1;
        }
        
        position += 0.0001f * normal;
        return true;
    }

    if (distance < r2) {
        // Distance to intersection
        float di = length(cProj - ray.o) - sqrt(r2 - distance);

        position = ray.o + ray.d * di;
        normal = normalize(position - center);
        ray.maxt = di;
        
        position += 0.0001f * normal;
		return true;
    }
    return false;
}
*/


void main() {
	// Get fragments 2D-coordinate on point in [-1, 1]
	vec2 pointCoord = vec2(2*(gl_PointCoord.x-0.5), 2*(-gl_PointCoord.y+0.5));
	// Get distance from middle of point to fragment
	float length = length(pointCoord);
	if (length > 1.0) discard; // Keep the particle round

	// Get normal of fragment on the imagined sphere
	vec3 normal = normalize(vec3(pointCoord, cos(length * M_PI / 2)));
	// Get fragment position in viewspace
	vec3 fragPos = centerPos + (particleSize/300) * vec3(normal.x, -normal.y, -normal.z);
	// Calculate direct diffuse lighting
	float diffuse = max(0.0, dot(normalize(viewSpaceLightPos - fragPos), normal));

	outColor = vec4((0.2 + diffuse) * color, 1.0);
}
