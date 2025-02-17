#include "Renderer.h"
#include "TestTriangle.h"

#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"  // Needed for FindWindowByName


#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

test::TestTriangle::TestTriangle()
    :m_Proj(),
    m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f))),
    m_Spinning(false), m_Translation(0), m_Rotation(0), 
    m_Scale(1.0f), m_MoveAngle(0.0f), m_MoveRadius(0.5f),
    m_Position(0.0f, 0.0f, 0.0f), m_WindowWidth(0), m_WindowHeight(0)
{
    const char* windowName = "Scene";
    ImGuiWindow* imguiWindow = ImGui::FindWindowByName(windowName);
    if (imguiWindow) {
        m_WindowWidth = static_cast<int>(imguiWindow->Size.x);
        m_WindowHeight = static_cast<int>(imguiWindow->Size.y);

        // Update projection matrix
        UpdateProjectionMatrix();
    }
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

    // Example vertices for the triangle
    glm::vec3 v0(-0.5f, -0.5f, 0.0f);
    glm::vec3 v1(0.5f, -0.5f, 0.0f);
    glm::vec3 v2(0.0f, 0.5f, 0.0f);

    glm::vec3 center = (v0 + v1 + v2) / 3.0f;

    m_Translation = center;
    m_Position = center;

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
    if (m_Spinning) {
        m_Rotation += 1.5f * deltaTime; // Adjust speed (1.5 radians per second)

        if (m_Rotation > glm::two_pi<float>()) {
            m_Rotation -= glm::two_pi<float>(); // Keep within [0, 2pi]
        }
    }

    if (m_Moving) {
        m_MoveAngle += 1.0f * deltaTime; // Move at steady speed
        if (m_MoveAngle > glm::two_pi<float>()) {
            m_MoveAngle -= glm::two_pi<float>(); // Keep within [0, 2π]
        }

        // Compute circular movement
        m_Position.x = m_MoveRadius * cos(m_MoveAngle);
        m_Position.y = m_MoveRadius * sin(m_MoveAngle);
    }
}

void test::TestTriangle::OnWindowResize(int width, int height) {
    m_WindowWidth = width;
    m_WindowHeight = height;
    GLCallV(glViewport(0, 0, width, height));
    UpdateProjectionMatrix();
}

void test::TestTriangle::UpdateProjectionMatrix() {
    float aspectRatio = static_cast<float>(m_WindowWidth) / m_WindowHeight;
    m_Proj = glm::ortho(-1.0f * aspectRatio, 1.0f * aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);
}

void test::TestTriangle::OnRender()
{
    GLCallV(glClearColor(0.5f, 0.5f, 0.5f, 1.0f));
    GLCallV(glClear(GL_COLOR_BUFFER_BIT));

    Renderer renderer;


    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, m_Position);
    model = glm::translate(model, m_Translation);
    model = glm::scale(model, glm::vec3(m_Scale, m_Scale, 1.0f)); // Scale
    model = glm::rotate(model, m_Rotation, glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate
    model = glm::translate(model, -m_Translation);

    glm::mat4 mvp = m_Proj * m_View * model;

    m_Shader->Bind();
    m_Shader->SetUniformMat4f("u_MVP", mvp);
    renderer.Draw(*m_VAO, *m_IBO, *m_Shader);
}

void test::TestTriangle::OnImGuiRender()
{
    // Create a button that toggles the spinning flag.
    if (ImGui::Button("Toggle Spin")) {
        m_Spinning = !m_Spinning;
    }

    if (ImGui::Button("Toggle Move")) {
        m_Moving = !m_Moving;
    }

    ImGui::Text("Spinning: %s", m_Spinning ? "Yes" : "No");
    ImGui::Text("Moving: %s", m_Moving ? "Yes" : "No");
    ImGui::Text("Rotation: %.2f degrees", glm::degrees(m_Rotation));

    ImGui::SliderFloat("Scale", &m_Scale, 0.1f, 3.0f, "%.2f");
    ImGui::SliderFloat("Move Radius", &m_MoveRadius, 0.1f, 1.0f, "%.2f");

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}
