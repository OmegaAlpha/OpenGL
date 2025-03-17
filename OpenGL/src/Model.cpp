#include "Model.h"
#include "Mesh.h"

Model::Model(const std::string& path) {
    // Use OBJLoader to parse the .obj file
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    if (OBJLoader::LoadOBJ(path, vertices, indices)) {
        // Create and store the Mesh object as a unique pointer
        m_Meshes.push_back(std::make_unique<Mesh>(vertices, indices));
    }
    else {
        std::cerr << "Failed to load model: " << path << std::endl;
    }
}

void Model::Draw(Shader& shader) {
    for (auto& mesh : m_Meshes) {
        mesh->Draw(shader);  // Use `->` since we now store unique pointers
    }
}