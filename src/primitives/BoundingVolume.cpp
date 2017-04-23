#include "BoundingVolume.h"

bool BoundingVolume::isSphereInside(glm::vec3 origin, float radius)
{
    
    float d = 0;
    float e = 0;
    // This can use SSE, see Thomas-akenine möller page 764
    // This algorithm is from the same page
    for (int i = 0; i < 3; i++) {
        if ((e = origin[i] - corner[i]) < 0) {
            if (e < -radius) return false;
            d += e*e;
        }
        else if ((e = origin[i] - (corner[i] + length)) > 0)
        {
            if (e > radius) return false;
            d += e*e;
        }
    }
    if (d > radius*radius) return false;
    return true;
}
