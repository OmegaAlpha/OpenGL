#include "Renderer.h"
#include "TestShaderToy.h"

#include "imgui/imgui.h"
#include <imgui/imgui_internal.h>
#include "imgui/TextEditor.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace fs = std::filesystem;

test::TestShaderToy::TestShaderToy()
    :m_Proj(), 
    m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f))),
    m_TranslationA(0.0f, 0.0f, 0), m_FrameCount(0)
{
    // Initialize the text editor
    m_Editor.SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());

    LoadShaderFiles("res/shader/Shadertoy/");
    if (!m_ShaderFiles.empty()) {
        LoadShader(m_ShaderFiles[0]);
    }

    // These are the vertex data we need to draw a rectangle from two triangles
    float positions[] = {
        -1.0f, -1.0f,   0.0f, 0.0f,// 0 positions, texture coords
         1.0f, -1.0f,   1.0f, 0.0f,// 1
         1.0f,  1.0f,   1.0f, 1.0f,// 2
        -1.0f,  1.0f,   0.0f, 1.0f,// 3
    };

    // These are the indices of our positions in the order we want to use to draw a rectangle
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };


    GLCallV(glEnable(GL_BLEND));
    GLCallV(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    // Vertex Array object
    m_VAO = std::make_unique<VertexArray>();

    // Vertex buffer object
    m_VBO = std::make_unique<VertexBuffer>(positions, 4 * 4 * sizeof(float)); // 4 vertices with 4 floats per vertex

    // Creating the layout for our data
    VertexBufferLayout layout;
    layout.Push<float>(2); // Our data consists of 2 floats (position coordinates)
    layout.Push<float>(2); // And 2 more floats (texture coordinates)
    m_VAO->AddBuffer(*m_VBO, layout);

    // Index buffer object
    m_IBO = std::make_unique<IndexBuffer>(indices, 6);


    // Load shader
    m_Shader = std::make_unique<Shader>(m_SelectedShader);

    // Shaders
    //m_Shader = std::make_unique<Shader>("res/shader/Shadertoy/Raymarching_primitives.shader");
    //m_Shader = std::make_unique<Shader>("res/shader/Shadertoy/Raytracer.shader");
    //m_Shader = std::make_unique<Shader>("res/shader/Shadertoy/uv_test.shader");
    //m_Shader = std::make_unique<Shader>("res/shader/Shadertoy/w10.shader");
    //m_Shader = std::make_unique<Shader>("res/shader/Shadertoy/Unreal_intro.shader");
    //m_Shader = std::make_unique<Shader>("res/shader/Shadertoy/Segmented_spiral.shader");
    //m_Shader = std::make_unique<Shader>("res/shader/Shadertoy/VolumetricIntegration.shader");


    if (m_Shader != nullptr) {
        m_Shader->Bind();
    }
    

    m_Editor.SetText(m_ShaderSource);
}

test::TestShaderToy::~TestShaderToy(){
}

void test::TestShaderToy::OnUpdate(float deltaTime){
}

void test::TestShaderToy::OnWindowResize(int width, int height) {
    GLCallV(glViewport(0, 0, width, height));
    m_WindowWidth = width;
    m_WindowHeight = height;
}

void test::TestShaderToy::UpdateProjectionMatrix() {

}

void test::TestShaderToy::OnMouseMove(float x, float y)
{
    m_MouseX = x;
    m_MouseY = y;
}

void test::TestShaderToy::LoadShaderFiles(const std::string& directory)
{
    m_ShaderFiles.clear();

    if (!fs::exists(directory) || !fs::is_directory(directory)) {
        std::cerr << "Error: Directory does not exist: " << directory << std::endl;
        return;
    }

    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.path().extension() == ".shader") {
            m_ShaderFiles.emplace_back(entry.path().string());
        }
    }

    if (m_ShaderFiles.empty()) {
        std::cerr << "Warning: No shader files found in " << directory << std::endl;
    }
}

void test::TestShaderToy::LoadShader(const std::string& shaderPath)
{
    std::ifstream file(shaderPath);
    if (file) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        m_ShaderSource = buffer.str();
        m_SelectedShader = shaderPath;
    }
}

void test::TestShaderToy::ReloadShader()
{
    std::ofstream file(m_SelectedShader);
    file << m_ShaderSource;
    file.close();

    m_Shader = std::make_unique<Shader>(m_SelectedShader);
}


void test::TestShaderToy::OnRender()
{
	GLCallV(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
	GLCallV(glClear(GL_COLOR_BUFFER_BIT));

    Renderer renderer;

    m_Shader->Bind();
    m_Shader->SetUniform1f("iTime", glfwGetTime());
    m_Shader->SetUniform1i("iFrame", m_FrameCount);
    m_Shader->SetUniform2f("iResolution", (float) m_WindowWidth, (float)m_WindowHeight);
    if (mouseState.leftPressed && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
        m_Shader->SetUniform4f("iMouse", m_MouseX, m_MouseY, mouseState.x, mouseState.y);
    }
    else m_Shader->SetUniform4f("iMouse", m_MouseX, m_MouseY, 0.0f, 0.0f);
    
    renderer.Draw(*m_VAO, *m_IBO, *m_Shader);
    m_FrameCount++;
}

void test::TestShaderToy::OnImGuiRender()
{
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    for (const auto& shader : m_ShaderFiles) {
        if (ImGui::Selectable(shader.c_str(), shader == m_SelectedShader)) {
            LoadShader(shader);
            m_Shader = std::make_unique<Shader>(m_SelectedShader);
            if (m_Shader != nullptr) {
                m_Shader->Bind();
                m_Editor.SetText(m_ShaderSource);
            }
        }
    }

    ImGui::End();

    ImGui::Begin("Shader Editor");

    // Display text editor widget
    m_Editor.Render("GLSL Shader Editor");

    // Recompile shader if button pressed
    if (ImGui::Button("Recompile Shader")) {
        m_ShaderSource = m_Editor.GetText();
        ReloadShader(); //Save and load shader source from editor
    }

}
