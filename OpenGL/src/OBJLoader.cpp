#include "OBJLoader.h"
#include <iostream>
#include <fstream>
#include <sstream>

bool OBJLoader::LoadOBJ(const std::string& filePath, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) {
    std::ifstream file(filePath);
    if (!file) {
        std::cerr << "Failed to open OBJ file: " << filePath << std::endl;
        return false;
    }

    // Parse the OBJ file (simplified example)
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {  // Vertex position
            Vertex vertex;
            ss >> vertex.Position.x >> vertex.Position.y >> vertex.Position.z;
            vertices.push_back(vertex);
        }
        else if (prefix == "f") {  // Face indices
            unsigned int index;
            while (ss >> index) {
                indices.push_back(index - 1);  // OBJ indices start at 1
            }
        }
    }

    // checking the data (only for debug)
    //std::cout << "Vertices: " << vertices.size() << ", Indices: " << indices.size() << std::endl;
    //for (const auto& v : vertices) {
    //    std::cout << "Vertex: (" << v.Position.x << ", " << v.Position.y << ", " << v.Position.z << ")\n";
    //}

    //std::cout << "Indices:\n";
    //for (size_t i = 0; i < indices.size(); i += 3) {
    //    std::cout << indices[i] << ", " << indices[i + 1] << ", " << indices[i + 2] << "\n";
    //}

    return true;
}