#include "Mesh.h"
#include "Renderer.h" // For calling renderer.Draw()

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
    : vertices(vertices), indices(indices)
{
    SetupMesh();
}

void Mesh::SetupMesh() {
    // Create our unique pointers for VAO, VBO, and IBO
    m_VAO = std::make_unique<VertexArray>();
    if (!vertices.empty()) {
        m_VBO = std::make_unique<VertexBuffer>(vertices.data(), vertices.size() * sizeof(Vertex));
    }
    else {
        std::cerr << "Error: vertices is empty, cannot create VertexBuffer.\n";
    }
    m_IBO = std::make_unique<IndexBuffer>(indices.data(), indices.size());

    // Set up the layout for the vertex data.
    // For example, if Vertex consists of Position (vec3), Normal (vec3), TexCoords (vec2):
    VertexBufferLayout layout;
    layout.Push<float>(3); // Position: 3 floats
    // Uncomment the next two lines if your Vertex struct has normals and texture coordinates.
    layout.Push<float>(3); // Normal: 3 floats
    layout.Push<float>(2); // TexCoords: 2 floats

    // Add the vertex buffer to the VAO with the layout
    m_VAO->AddBuffer(*m_VBO, layout);
}

void Mesh::Draw(Shader& shader) {
    // Bind the shader
    shader.Bind();

    // Bind the VAO (which holds our vertex attribute configuration)
    m_VAO->Bind();

    // Create or use your Renderer object to draw the mesh.
    // This assumes you have a Renderer::Draw() function that accepts a VAO, IBO, and shader.
    Renderer renderer;
    renderer.Draw(*m_VAO, *m_IBO, shader);

    // Optionally unbind VAO after drawing
    m_VAO->Unbind();
}