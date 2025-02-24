#pragma once

#include "Test.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"

#include "imgui/TextEditor.h"

#include <memory>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>


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
		void LoadShaderFiles(const std::string& directory);
		void LoadShader(const std::string& shaderPath);
		void ReloadShader();

	private:
		std::unique_ptr <VertexArray> m_VAO;
		std::unique_ptr <VertexBuffer> m_VBO;
		std::unique_ptr <IndexBuffer> m_IBO;
		std::unique_ptr <Texture> m_Texture;

		std::vector<std::string> m_ShaderFiles;
		std::string m_SelectedShader;
		std::string m_ShaderSource;
		std::unique_ptr <Shader> m_Shader;

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