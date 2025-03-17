#pragma once

#include <vector>
#include "Vertex.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "Shader.h"

class Mesh {
public:
    // Mesh vertex and index data
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Constructor: takes the vertex and index data
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

    // Draw the mesh using the provided shader
    void Draw(Shader& shader);

private:
    // Unique pointers to our OpenGL buffer objects
    std::unique_ptr<VertexArray> m_VAO;
    std::unique_ptr<VertexBuffer> m_VBO;
    std::unique_ptr<IndexBuffer> m_IBO;

    // Setup the VAO/VBO/IBO and link vertex attributes
    void SetupMesh();
};