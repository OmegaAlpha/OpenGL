#include <vector>
#include <array>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <glm/glm.hpp>
#include "Vertex.h"
#include <OBJLoader.h>

// Hash function for Vertex to use in unordered_map
struct VertexHash {
    std::size_t operator()(const Vertex& v) const {
        size_t seed = 0;
        auto hashCombine = [&](size_t hash) { seed ^= hash + 0x9e3779b9 + (seed << 6) + (seed >> 2); };

        hashCombine(std::hash<float>{}(v.Position.x));
        hashCombine(std::hash<float>{}(v.Position.y));
        hashCombine(std::hash<float>{}(v.Position.z));
        hashCombine(std::hash<float>{}(v.Normal.x));
        hashCombine(std::hash<float>{}(v.Normal.y));
        hashCombine(std::hash<float>{}(v.Normal.z));
        hashCombine(std::hash<float>{}(v.TexCoords.x));
        hashCombine(std::hash<float>{}(v.TexCoords.y));

        return seed;
    }
};

// Comparison function for unordered_map
struct VertexEqual {
    bool operator()(const Vertex& a, const Vertex& b) const {
        return a.Position == b.Position && a.Normal == b.Normal && a.TexCoords == b.TexCoords;
    }
};

bool OBJLoader::LoadOBJ(const std::string& path, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) {
    std::vector<glm::vec3> temp_positions;
    std::vector<glm::vec3> temp_normals;
    std::vector<glm::vec2> temp_texCoords;

    std::unordered_map<Vertex, unsigned int, VertexHash, VertexEqual> vertexMap;

    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open OBJ file: " << path << std::endl;
        return false;
    }

    // Estimate file size and reserve memory accordingly
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    size_t estimatedVertices = fileSize / 50;
    size_t estimatedFaces = fileSize / 60;

    temp_positions.reserve(estimatedVertices);
    temp_normals.reserve(estimatedVertices);
    temp_texCoords.reserve(estimatedVertices);
    vertices.reserve(estimatedVertices);
    indices.reserve(estimatedFaces * 3);  // Assume mostly triangles

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::istringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "v") {  // Vertex position
            glm::vec3 pos;
            ss >> pos.x >> pos.y >> pos.z;
            temp_positions.emplace_back(pos);
        }
        else if (type == "vt") {  // Texture coordinate
            glm::vec2 tex;
            ss >> tex.x >> tex.y;
            tex.y = 1.0f - tex.y;  // Flip Y for OpenGL
            temp_texCoords.emplace_back(tex);
        }
        else if (type == "vn") {  // Normal
            glm::vec3 normal;
            ss >> normal.x >> normal.y >> normal.z;
            temp_normals.emplace_back(normal);
        }

        else if (type == "f") {  // Face (triangles & quads)
            std::vector<unsigned int> vIndices, tIndices, nIndices;
            std::string faceVertex;
            while (ss >> faceVertex) {
                std::istringstream faceStream(faceVertex);
                unsigned int vIndex, tIndex = 0, nIndex = 0;
                char slash;

                faceStream >> vIndex;
                if (faceStream.peek() == '/') {
                    faceStream >> slash;
                    if (faceStream.peek() != '/') faceStream >> tIndex;
                    if (faceStream.peek() == '/') {
                        faceStream >> slash >> nIndex;
                    }
                }

                // Convert to zero-based indexing
                vIndex--;
                if (tIndex > 0) tIndex--;
                if (nIndex > 0) nIndex--;

                vIndices.push_back(vIndex);
                tIndices.push_back(tIndex);
                nIndices.push_back(nIndex);
            }

            // Convert quad to two triangles
            if (vIndices.size() == 4) {
                std::array<int, 6> quadToTri = { 0, 1, 2, 0, 2, 3 };
                for (int i : quadToTri) {
                    Vertex vertex;
                    vertex.Position = temp_positions[vIndices[i]];
                    if (!temp_normals.empty() && nIndices[i] < temp_normals.size()) {
                        vertex.Normal = temp_normals[nIndices[i]];
                    }
                    if (!temp_texCoords.empty() && tIndices[i] < temp_texCoords.size()) {
                        vertex.TexCoords = temp_texCoords[tIndices[i]];
                    }

                    auto it = vertexMap.find(vertex);
                    if (it != vertexMap.end()) {
                        indices.emplace_back(it->second);
                    }
                    else {
                        unsigned int newIndex = static_cast<unsigned int>(vertices.size());
                        vertices.emplace_back(vertex);
                        indices.emplace_back(newIndex);
                        vertexMap[vertex] = newIndex;
                    }
                }
            }
            // Handle normal triangles
            else if (vIndices.size() == 3) {
                for (int i = 0; i < 3; i++) {
                    Vertex vertex;
                    vertex.Position = temp_positions[vIndices[i]];
                    if (!temp_normals.empty() && nIndices[i] < temp_normals.size()) {
                        vertex.Normal = temp_normals[nIndices[i]];
                    }
                    if (!temp_texCoords.empty() && tIndices[i] < temp_texCoords.size()) {
                        vertex.TexCoords = temp_texCoords[tIndices[i]];
                    }

                    auto it = vertexMap.find(vertex);
                    if (it != vertexMap.end()) {
                        indices.emplace_back(it->second);
                    }
                    else {
                        unsigned int newIndex = static_cast<unsigned int>(vertices.size());
                        vertices.emplace_back(vertex);
                        indices.emplace_back(newIndex);
                        vertexMap[vertex] = newIndex;
                    }
                }
            }
         
        }
    }

    //std::cout << "Final Vertices: " << vertices.size() << std::endl;
    //for (size_t i = 0; i < vertices.size(); i++) {
    //    std::cout << i << ": Pos(" << vertices[i].Position.x << ", " << vertices[i].Position.y << ", " << vertices[i].Position.z
    //        << ") Norm(" << vertices[i].Normal.x << ", " << vertices[i].Normal.y << ", " << vertices[i].Normal.z
    //        << ") Tex(" << vertices[i].TexCoords.x << ", " << vertices[i].TexCoords.y << ")" << std::endl;
    //}

    //std::cout << "Final Indices: " << indices.size() << std::endl;
    //for (size_t i = 0; i < indices.size(); i += 3) {
    //    std::cout << "Triangle: " << indices[i] << ", " << indices[i + 1] << ", " << indices[i + 2] << std::endl;
    //}

    file.close();
    return true;
}
