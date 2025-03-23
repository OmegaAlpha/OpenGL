#include "TestModelLoading.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"  // Needed for FindWindowByName
#include "ImGuiFileDialog/ImGuiFileDialog.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtx/string_cast.hpp>

namespace test {

    TestModelLoading::TestModelLoading()
        : m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.5f, -6.0f))),
        m_Translation(0.0f, 0.0f, 0.0f), m_modelScale(1.0f),
        m_ModelLoaded(false)
    {
        const char* windowName = "Scene";
        ImGuiWindow* imguiWindow = ImGui::FindWindowByName(windowName);
        if (imguiWindow) {
            m_WindowWidth = static_cast<int>(imguiWindow->Size.x);
            m_WindowHeight = static_cast<int>(imguiWindow->Size.y);

            // Update projection matrix
            UpdateProjectionMatrix();
        }
        m_Model = std::make_unique<Model>("res/models/teapot.obj");
        if (m_Model) {
            m_ModelLoaded = true;
        }

        m_Shader = std::make_unique<Shader>("res/shader/model_shader.shader");
        if (m_Shader) {
            m_Shader->Bind();
        }

        glEnable(GL_DEPTH_TEST); // Enable z-checking
        glDepthFunc(GL_LESS);    // draw closest on top (default)
    }

    TestModelLoading::~TestModelLoading() {
        glDisable(GL_DEPTH_TEST); // Disable z-checking for the other tests
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Turn back on fill mode for the other tests
    }

    void TestModelLoading::OnUpdate(float deltaTime) {
        // Update any animations or transformations here
    }

    void TestModelLoading::OnRender() {
        GLCallV(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        if (m_ModelLoaded) {

            glm::mat4 m_modelMatrix = glm::mat4(1.0f);  // Identity matrix

            m_Shader->Bind();

            m_modelMatrix = glm::translate(m_modelMatrix, m_Translation);
            m_modelMatrix = glm::scale(m_modelMatrix, glm::vec3(m_modelScale));
            m_Shader->SetUniformMat4f("u_Model", m_modelMatrix);
            m_Shader->SetUniformMat4f("u_View", m_View);
            m_Shader->SetUniformMat4f("u_Projection", m_Proj);

            m_Shader->SetUniform3f("u_Color", 1.0f, 1.0f, 1.0f);
            m_Shader->SetUniform3f("lightPos", 10.0f, 10.0f, 10.0f);
            m_Shader->SetUniform3f("lightColor", 1.0f, 1.0f, 1.0f);
            m_Shader->SetUniform3f("objectColor", 0.6f, 0.6f, 0.6f); // Example color

            //std::cout << glm::to_string(m_modelMatrix) << std::endl; DEBUG print modelmatrix
            //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe on
            //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Wireframe off

            m_Model->Draw(*m_Shader);
        }
    }

    void TestModelLoading::OnImGuiRender() {
        ImGui::Text("Model Loading Test");
        if (m_ModelLoaded) {
            ImGui::Text("Model loaded successfully!");
        }
        else {
            ImGui::Text("Failed to load model.");
        }
        if (ImGui::Button("Load Model")) {
            ImGuiFileDialog::Instance()->OpenDialog("ChooseFile", "Select OBJ File", ".obj", IGFD::FileDialogConfig("."));
        }

        // Display file dialog
        if (ImGuiFileDialog::Instance()->Display("ChooseFile")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
                m_Model->LoadModel(filePath);
            }
            ImGuiFileDialog::Instance()->Close();
        }
        ImGui::SliderFloat("Scale", &m_modelScale, 0.1f, 5.0f); // Scale from 0.1x to 100x
        ImGui::SliderFloat3("Translation", &m_Translation.x, -5.0f, 5.0f);
        static bool wireframe = false;
        if (ImGui::Checkbox("Wireframe Mode", &wireframe)) {
            glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
        }
    }

    void TestModelLoading::OnWindowResize(int width, int height){
        m_WindowWidth = width;
        m_WindowHeight = height;
        GLCallV(glViewport(0, 0, width, height));
        UpdateProjectionMatrix();
    }

    void TestModelLoading::UpdateProjectionMatrix(){
        float aspectRatio = static_cast<float>(m_WindowWidth) / m_WindowHeight;
        m_Proj = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);

    }

}