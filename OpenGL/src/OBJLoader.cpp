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
bool OBJLoader::LoadOBJ(const std::string& path, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, bool computeFaceNormals, bool computeVertexNormals) {
    std::vector<glm::vec3> temp_positions;
    std::vector<glm::vec3> temp_normals;
    std::vector<glm::vec2> temp_texCoords;

    std::unordered_map<Vertex, unsigned int, VertexHash, VertexEqual> vertexMap;

    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open OBJ file: " << path << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::istringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "v") {  // Vertex position
            glm::vec3 pos;
            ss >> pos.x >> pos.y >> pos.z;
            temp_positions.push_back(pos);
        }
        else if (type == "vt") {  // Texture coordinate
            glm::vec2 tex;
            ss >> tex.x >> tex.y;
            tex.y = 1.0f - tex.y;  // Flip Y for OpenGL
            temp_texCoords.push_back(tex);
        }
        else if (type == "vn") {  // Normal
            glm::vec3 normal;
            ss >> normal.x >> normal.y >> normal.z;
            temp_normals.push_back(normal);
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

            std::vector<int> quadToTri = { 0, 1, 2, 0, 2, 3 };
            std::vector<int> faceIndices = (vIndices.size() == 4) ? quadToTri : std::vector<int>{ 0, 1, 2 };

            glm::vec3 faceNormal;
            if (computeFaceNormals) {
                // Compute face normal for flat shading
                glm::vec3 edge1 = temp_positions[vIndices[1]] - temp_positions[vIndices[0]];
                glm::vec3 edge2 = temp_positions[vIndices[2]] - temp_positions[vIndices[0]];
                faceNormal = glm::normalize(glm::cross(edge1, edge2));
            }

            for (int i : faceIndices) {
                Vertex vertex;
                vertex.Position = temp_positions[vIndices[i]];

                if (!computeFaceNormals && !temp_normals.empty() && nIndices[i] < temp_normals.size()) {
                    vertex.Normal = temp_normals[nIndices[i]];  // Use normals from file
                }
                else {
                    vertex.Normal = faceNormal;  // Use computed face normal
                }

                if (!temp_texCoords.empty() && tIndices[i] < temp_texCoords.size()) {
                    vertex.TexCoords = temp_texCoords[tIndices[i]];
                }

                auto it = vertexMap.find(vertex);
                if (it != vertexMap.end()) {
                    indices.push_back(it->second);
                }
                else {
                    unsigned int newIndex = static_cast<unsigned int>(vertices.size());
                    vertices.push_back(vertex);
                    indices.push_back(newIndex);
                    vertexMap[vertex] = newIndex;
                }
            }
        }
    }

    // If we need vertex normals and they are not in the file, compute them
    if (computeVertexNormals && temp_normals.empty()) {
        std::vector<glm::vec3> computedVertexNormals(vertices.size(), glm::vec3(0.0f));

        // Accumulate normals for each vertex
        for (size_t i = 0; i < indices.size(); i += 3) {
            unsigned int i0 = indices[i], i1 = indices[i + 1], i2 = indices[i + 2];
            glm::vec3 normal = glm::normalize(glm::cross(
                vertices[i1].Position - vertices[i0].Position,
                vertices[i2].Position - vertices[i0].Position
            ));
            computedVertexNormals[i0] += normal;
            computedVertexNormals[i1] += normal;
            computedVertexNormals[i2] += normal;
        }

        // Normalize the accumulated normals
        for (size_t i = 0; i < vertices.size(); ++i) {
            vertices[i].Normal = glm::normalize(computedVertexNormals[i]);
        }
    }

    file.close();
    return true;
}
