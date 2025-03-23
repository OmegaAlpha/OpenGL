#pragma once

#include <vector>
#include <string>
#include "Vertex.h"

class OBJLoader {
public:
    static bool LoadOBJ(const std::string& filePath, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, bool computeFaceNormals, bool computeVertexNormals);
};