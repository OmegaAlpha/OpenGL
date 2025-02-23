#pragma once

#include "Test.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"

#include "imgui/TextEditor.h"

#include <memory>

namespace test {

	class TestShaderToy : public Test
	{
	public:
		TestShaderToy();
		~TestShaderToy();

		void OnUpdate(float deltaTime) override;
		void OnWindowResize(int width, int height) override;
		void OnRender() override;
		void OnImGuiRender() override;
		void UpdateProjectionMatrix();
		void OnMouseMove(float x, float y) override;
	private:
		std::unique_ptr <VertexArray> m_VAO;
		std::unique_ptr <VertexBuffer> m_VBO;
		std::unique_ptr <IndexBuffer> m_IBO;
		std::unique_ptr <Shader> m_Shader;
		std::unique_ptr <Texture> m_Texture;

		glm::mat4 m_Proj, m_View;
		glm::vec3 m_TranslationA;
		int m_WindowWidth, m_WindowHeight;
		int m_FrameCount;

		TextEditor m_Editor;
		std::string m_FragmentShaderSource = R"(  
#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0, 0.0, 0.0, 1.0); // Default Red
})";

	};
}