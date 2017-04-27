#pragma once

#include <glm/common.hpp>

class BoundingVolume
{
public:
    /* Assume volume is a cube */
    glm::vec3 corner;
    float length;

    BoundingVolume() { corner = glm::vec3(0); length = 0; }
    BoundingVolume(glm::vec3 corner, float length) { this->corner = corner; this->length = length; }
    bool isSphereInside(glm::vec3 origin, float radius);
    glm::vec3 getCenter() { return corner + glm::vec3(length / 2); }
    float getVolume() { return length*length*length; }
};