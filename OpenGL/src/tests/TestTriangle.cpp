#include "Renderer.h"
#include "TestTriangle.h"

#include "imgui/imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

test::TestTriangle::TestTriangle()
    :m_Proj(glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f)),
    m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)))
{
    // These are the needed to draw a triangle with vertex colors
    float vertices[] = {
       -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom left (blue)
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom right (green)
        0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f  // top middle (red)
    };
    // These are the indices of our positions in the order we want to use to draw a rectangle
    unsigned int indices[] = {
        0, 1, 2
    };


    // Vertex Array object
    m_VAO = std::make_unique<VertexArray>();

    // Vertex buffer object
    m_VBO = std::make_unique<VertexBuffer>(vertices, 3 * 6 * sizeof(float)); // 3 vertices with 6 floats per vertex

    // Creating the layout for our data
    VertexBufferLayout layout;
    layout.Push<float>(3); // Our data consists of 3 floats for position
    layout.Push<float>(3); // and then another 3 floats for color
    m_VAO->AddBuffer(*m_VBO, layout);

    // Index buffer object
    m_IBO = std::make_unique<IndexBuffer>(indices, 3);

    // Shaders
    m_Shader = std::make_unique<Shader>("res/shader/FragColor.shader");
    m_Shader->Bind();
}

test::TestTriangle::~TestTriangle() {
}

void test::TestTriangle::OnUpdate(float deltaTime) {
}

void test::TestTriangle::OnRender()
{
    GLCallV(glClearColor(0.5f, 0.5f, 0.5f, 1.0f));
    GLCallV(glClear(GL_COLOR_BUFFER_BIT));

    Renderer renderer;

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    glm::mat4 mvp = m_Proj * m_View * model; // Matrix multiplication in reverse order because of column major ordering in memory
    m_Shader->Bind();
    renderer.Draw(*m_VAO, *m_IBO, *m_Shader);


}

void test::TestTriangle::OnImGuiRender()
{
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}
