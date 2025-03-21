#pragma once

#include "Test.h"
#include "Renderer.h"
#include "Model.h"

#include <memory>

namespace test {

    class TestModelLoading : public Test {
    private:
        std::unique_ptr<Model> m_Model; // Store the loaded model
        std::unique_ptr<Shader> m_Shader; // Shader for rendering
        glm::mat4 m_Proj, m_View, m_modelMatrix;
        glm::vec3 m_Translation;
        bool m_ModelLoaded;
        float m_modelScale;

        int m_WindowWidth, m_WindowHeight;

    public:
        TestModelLoading();
        ~TestModelLoading();

        void OnUpdate(float deltaTime) override;
        void LoadModel(const std::string& path);
        void OnRender() override;
        void OnImGuiRender() override;
        void OnWindowResize(int width, int height) override;
        void UpdateProjectionMatrix();
    };

}