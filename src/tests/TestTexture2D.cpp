#include "Renderer.h"
#include "TestTexture2D.h"

#include "imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

test::TestTexture2D::TestTexture2D()
    :m_Proj(glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f)), 
    m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f))),
    m_TranslationA(200, 200, 0), m_TranslationB(400, 200, 0)
{
    // These are the vertex data we need to draw a rectangle from two triangles
    float positions[] = {
        -100.0f, -75.0f, 0.0f, 0.0f,// 0 positions, texture coords
         100.0f, -75.0f, 1.0f, 0.0f,// 1
         100.0f,  75.0f, 1.0f, 1.0f,// 2
        -100.0f,  75.0f, 0.0f, 1.0f,// 3
    };

    // These are the indices of our positions in the order we want to use to draw a rectangle
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };


    GLCallV(glEnable(GL_BLEND));
    GLCallV(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    glDisable(GL_DEPTH_TEST); // Disable z-checking for this test (makes the textures draw in the same layer)

    // Vertex Array object
    m_VAO = std::make_unique<VertexArray>();

    // Vertex buffer object
    m_VBO = std::make_unique<VertexBuffer>(positions, 4 * 4 * sizeof(float)); // 4 vertices with 4 floats per vertex

    // Creating the layout for our data
    VertexBufferLayout layout;
    layout.Push<float>(2); // Our data consists of 2 floats
    layout.Push<float>(2); // And 2 more floats
    m_VAO->AddBuffer(*m_VBO, layout);

    // Index buffer object
    m_IBO = std::make_unique<IndexBuffer>(indices, 6);

    // Shaders
    m_Shader = std::make_unique<Shader>("res/shader/Basic.shader");
    m_Shader->Bind();
    m_Shader->SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

    m_Texture = std::make_unique<Texture>("res/textures/DVD_video.png");
    
    m_Shader->SetUniform1i("u_Texture", 0);
}

test::TestTexture2D::~TestTexture2D(){
}

void test::TestTexture2D::OnUpdate(float deltaTime){
}

void test::TestTexture2D::OnWindowResize(int width, int height) {
    GLCallV(glViewport(0, 0, width, height));
    m_WindowWidth = width;
    m_WindowHeight = height;
    UpdateProjectionMatrix();
}

void test::TestTexture2D::UpdateProjectionMatrix() {
    m_Proj = glm::ortho(0.0f, static_cast<float>(m_WindowWidth), 0.0f, static_cast<float>(m_WindowHeight), -1.0f, 1.0f);
}

void test::TestTexture2D::OnRender()
{
	GLCallV(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    GLCallV(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    Renderer renderer;


    m_Texture->Bind(0);

    {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationA);
        glm::mat4 mvp = m_Proj * m_View * model; // Matrix multiplication in reverse order because of column major ordering in memory
        m_Shader->Bind();
        m_Shader->SetUniformMat4f("u_MVP", mvp);
        renderer.Draw(*m_VAO, *m_IBO, *m_Shader);
    }

    {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationB);
        glm::mat4 mvp = m_Proj * m_View * model; // Matrix multiplication in reverse order because of column major ordering in memory
        m_Shader->Bind();
        m_Shader->SetUniformMat4f("u_MVP", mvp);
        renderer.Draw(*m_VAO, *m_IBO, *m_Shader);
    }
}

void test::TestTexture2D::OnImGuiRender()
{
    ImGui::SliderFloat2("Translation A", &m_TranslationA.x, 0.0f, 800.0f);
    ImGui::SliderFloat2("Translation B", &m_TranslationB.x, 0.0f, 800.0f);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}
