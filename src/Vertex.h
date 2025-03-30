#pragma once

#include "glm/glm.hpp"  // For vec3 and vec2

struct Vertex {
    glm::vec3 Position;   // 3D position
    glm::vec3 Normal;     // Normal vector
    glm::vec2 TexCoords;  // Texture coordinates
};