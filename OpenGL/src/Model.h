#pragma once

#include <vector>
#include <memory>
#include <string>
#include "Mesh.h"
#include "OBJLoader.h"

class Model {
public:
    Model(const std::string& path);  // Constructor to load a model from a file
    void Draw(Shader& shader);       // Draw method for rendering

private:
    std::vector<std::unique_ptr<Mesh>> m_Meshes;    // Store loaded meshes
};