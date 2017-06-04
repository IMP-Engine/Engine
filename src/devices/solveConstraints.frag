#version 430

out vec3 newPos;

uniform sampler2DRect oldPos;
uniform sampler2DRect invmasses;
uniform sampler2DRect constraints; 
uniform isampler2DRect segments;
uniform isampler2DRect indirections;
uniform isampler2DRect boundConstraints;

uniform int iteration;
uniform float omega;

void main() {
    int start = texture(indirections, gl_FragCoord.xy).x;
    int end   = texture(indirections, gl_FragCoord.xy + vec2(1, 0)).x;

    vec3 oldPos1 = texture(oldPos, gl_FragCoord.xy).xyz;
    float w1 = texture(invmasses, gl_FragCoord.xy).x;

    vec3 delta = vec3(0);

    for(; start < end; start++)
    {
        int constraintIndex = texelFetch(segments, ivec2(start, 0)).x;
        vec4 constraint = texelFetch(constraints, ivec2(constraintIndex, 0));
      
        int p2;
        if (int(constraint.x) == int(gl_FragCoord.x))
            p2 = int(constraint.y);
        else 
            p2 = int(constraint.x);
        
        vec3 oldPos2 = texelFetch(oldPos, ivec2(p2, 0)).xyz;
        float w2 = texelFetch(invmasses, ivec2(p2, 0)).x;
      
        // Compute
        vec3 diff = oldPos1 - oldPos2;
        float c = length(diff) - constraint.z;
        vec3 grad = normalize(diff);
        vec3 dDelta = w1 * c * grad / (w1 + w2);

        dDelta *= (1 - pow(1 - constraint.a, 1 / float(iteration)));
        dDelta *= omega / texture(boundConstraints, gl_FragCoord.xy).x;

        delta -=  dDelta;
    }
    
    newPos = oldPos1 + delta;
}